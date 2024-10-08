#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <sys/uio.h>
#include <cstdlib>
#include <variant>
#include <bitset>
#include "syscalls_job.h"
#include <cstring>
#include <filesystem>
#include <cassert>
#include "timerw.h"
#include "../utils/hlp.h"

#define MAX_ENTROPY_LENGTH_ARG 256

namespace jobs {

namespace internal {
struct diskstats
{
    unsigned long io_inflight; // currently in the queue
    unsigned long rd_iotime; // total read io time
    unsigned long wr_iotime; // total write io time
};

static diskstats rdstats(const char* devname);
}



syscalls_job::syscalls_job(const config_state& config, const diskctx* disk) noexcept
: job(config, disk)
{
    device_partition_ = disk_->partitions[config_.partition_id];
    mapping_dir_ = device_partition_->mntroot + "/iofprb";
    mapping_file_ = mapping_dir_ + "/" + get_tmp_file_name();
}

void syscalls_job::raise_err(const char* errmsg)
{
    std::string error = errmsg;
    error += strerror(errno);
    throw std::runtime_error(error);
}

syscalls_job::~syscalls_job() noexcept
{
    if(fd_)
    {
        unlink(mapping_file_.c_str());
        close(fd_);
        fd_ = 0;
    }
    rmdir(mapping_dir_.c_str());
}

static int fill_random(char* buff, uint size)
{
    auto tmp = static_cast<double>(size)/MAX_ENTROPY_LENGTH_ARG;
    int chunks = static_cast<int>(tmp);
    assert(tmp - chunks == 0.0);

    for(int i=0; i<chunks; i++)
        if(getentropy(buff + (MAX_ENTROPY_LENGTH_ARG * i), MAX_ENTROPY_LENGTH_ARG))
            return -1;

    return 0;
}

static int generate_content(int fd, uint size)
{
    char* buff = new char[size];
    if(fill_random(buff, size))
        return -1;

    lseek(fd, 0, SEEK_SET);
    uint written = write(fd, buff, size);
    if(written != size)
        return -1;

    fsync(fd);
    lseek(fd, 0, SEEK_SET);
    return 0;
}

static void create_dir(const char* dirname)
{
    struct stat st{};
    if(stat(dirname, &st) == -1)
        if(mkdir(dirname, 0755))
        {
            perror("allocation failed");
            abort();
        }
}


double syscalls_job::emit_requests(const std::shared_ptr<struct iovec>& iov,
                                   std::size_t num_blocks,
                                   struct io_uring& ring,
                                   __s32 butch_size,
                                   io_uring_prep_reqv_t prev_req)
{
    terminate_if_requested();

    timerw timer;
    timer.initialize();

    auto stats = internal::rdstats(disk_->short_devname.c_str());
    for (std::size_t i = 0; i < num_blocks; i++)
    {
        // first check if something in waiting queue
        int waiting_loop = 10;
        while(stats.io_inflight > 0 && waiting_loop-- > 0)
        {
            std::cout << "loop " << waiting_loop << std::endl;
            sleep(1);
            stats = internal::rdstats(disk_->short_devname.c_str());
        }

        off_t offset = i * butch_size;
        struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
        if (!sqe)
            raise_err("io_uring_get_sqe failed");

        prev_req(sqe, fd_, iov.get(), queue_count_, offset);
        sqe->flags |= IOSQE_IO_DRAIN;

        if (io_uring_submit(&ring) < 0)
            raise_err("io_uring_submit failed");

        timer.start();
        struct io_uring_cqe* cqe;
        if (io_uring_wait_cqe(&ring, &cqe) < 0)
            raise_err("io_uring_wait_cqe failed");

        if (cqe->res != butch_size)
            raise_err("read failed");

        timer.stop();
        io_uring_cqe_seen(&ring, cqe);
    }

    long nanos = timer.nanoseconds(num_blocks);
    timer.reset();

    double elapsed = static_cast<double>(nanos) / 1e9;
    return static_cast<double>(config_.get_alloc_chunk()) / elapsed / CNFG_1MG;
}


void syscalls_job::initialize_()
{
    update_phase("initializing");
    pgsize_ = sysconf(_SC_PAGESIZE);

    create_dir(mapping_dir_.c_str());
    fd_ = open(mapping_file_.c_str(), O_RDWR | O_DIRECT | O_NOATIME | O_NONBLOCK | O_TRUNC | O_CREAT);
    if (fd_ == -1)
        raise_err("Failed to open descriptor for the device file");

    uint block_size = config_.get_block_size();
    // double check
    buff_size_ = std::min(SYSJOB_DEFAULT_BUF_SZ, block_size);
    if(buff_size_ < block_size)
    {
        while(block_size % buff_size_ != 0)
            buff_size_ /= 2;
    }

    queue_count_ = block_size / buff_size_;
}

void syscalls_job::start_()
{
    update_phase("job setup");
    // allocate fs space + a bit extra
    if(generate_content(fd_, config_.get_alloc_chunk() + pgsize_))
        raise_err("allocation error");

    struct io_uring ring;
    if (io_uring_queue_init(queue_count_, &ring, IORING_SETUP_SINGLE_ISSUER) < 0)
        raise_err("io_uring_queue_init failed");

    uint iters = config_.get_iterations();
    {
        terminate_if_requested();

        // === reading
        update_phase("prepare reading task");
        // iover raii
        std::shared_ptr<struct iovec> iov_rd = std::shared_ptr<struct iovec>(
                static_cast<struct iovec*>(calloc(queue_count_, sizeof(struct iovec))),
                [q = queue_count_](struct iovec* ptr)
                {
                    for(std::size_t i = 0; i<q; i++)
                        free(ptr[i].iov_base);
                    free(ptr);
                });

        for(std::size_t i=0; i<queue_count_; i++)
        {
            iov_rd.get()[i].iov_base = aligned_alloc(pgsize_, buff_size_);
            iov_rd.get()[i].iov_len = buff_size_;
        }

        update_phase("run reading task");
        while(iters-- > 0)
        {
            __s32 read_butch = static_cast<__s32>(buff_size_ * queue_count_);
            std::size_t num_blocks = config_.get_alloc_chunk() / read_butch;
            double throughput = emit_requests(iov_rd, num_blocks, ring, read_butch, &io_uring_prep_readv);
            push_msg(job_msg{throughput, measure_type::READ});
        }
    }

    {
        terminate_if_requested();

        // === writing
        iters = config_.get_iterations();
        update_phase("prepare writing task");
        std::shared_ptr<struct iovec> iov_wr = std::shared_ptr<struct iovec>(
                static_cast<struct iovec*>(calloc(queue_count_, sizeof(struct iovec))),
                [q = queue_count_](struct iovec* ptr)
                {
                    for(std::size_t i = 0; i<q; i++)
                        free(ptr[i].iov_base);
                    free(ptr);
                });

        for(std::size_t i=0; i<queue_count_; i++)
        {
            auto& elem = iov_wr.get()[i];
            elem.iov_base = aligned_alloc(pgsize_, buff_size_);
            elem.iov_len = buff_size_;
            if(fill_random(static_cast<char*>(elem.iov_base), elem.iov_len))
                raise_err("write preparing fails");
        }
        update_phase("run writing task");
        while(iters-- > 0)
        {
            __s32 write_butch = static_cast<__s32>(buff_size_ * queue_count_);
            std::size_t num_blocks = config_.get_alloc_chunk() / write_butch;
            double throughput = emit_requests(iov_wr, num_blocks, ring, write_butch, &io_uring_prep_writev);
            push_msg(job_msg{throughput, measure_type::WRITE});
        }
    }

    io_uring_queue_exit(&ring);
}

namespace internal {
#define JOBS_INTERNAL_DISKSTATS_SYSFS "/sys/class/block/%s/stat"

static diskstats rdstats(const char* devname)
{
    // todo: store the sysfs file name within job
    char sysfs_block_filename[100];
    sprintf(sysfs_block_filename, JOBS_INTERNAL_DISKSTATS_SYSFS, devname);

    diskstats dstat{};
    std::cout << sysfs_block_filename << std::endl;
    auto fst = open(sysfs_block_filename, O_RDONLY);
    assert(fst != -1);

    char buf[256];
    read(fst, buf, sizeof(buf));
    sscanf(buf, " %*d %*d %*d %ld %*d %*d %*d %ld %ld ", &dstat.rd_iotime, &dstat.wr_iotime, &dstat.io_inflight);

    close(fst);
    return dstat;
}

}





} // jobs