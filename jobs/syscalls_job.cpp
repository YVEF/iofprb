#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <liburing.h>
#include <iostream>
#include <sys/uio.h>
#include <cstdlib>
#include <variant>
#include <bitset>
#include "syscalls_job.h"
#include <cstring>
#include "timerw.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <dirent.h>
#include <libgen.h>


namespace jobs {

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
        close(fd_);
        fd_ = 0;
    }
}

void syscalls_job::initialize_()
{
    auto device_partition = disk_->partitions[config_.partition_id];
    pgsize_ = sysconf(_SC_PAGESIZE);

    fd_ = open(device_partition->devname.c_str(), O_RDWR | O_DIRECT | O_NOATIME);
    if (fd_ == -1)
        raise_err("Failed to open descriptor for the device");

    queue_count_ = config_.get_block_size();
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
    struct io_uring ring;
    if (io_uring_queue_init(queue_count_, &ring, IORING_SETUP_SINGLE_ISSUER) < 0)
        raise_err("io_uring_queue_init failed");

    uint iters = config_.get_iterations();
    while(iters-- > 0)
    {
        // iover raii
        std::shared_ptr<struct iovec> iov = std::shared_ptr<struct iovec>(
                static_cast<struct iovec*>(calloc(queue_count_, sizeof(struct iovec))),
                        [q = queue_count_](struct iovec* ptr)
                {
                    for(std::size_t i = 0; i<q; i++)
                        free(ptr[i].iov_base);
                    free(ptr);
                });

        for(std::size_t i=0; i<queue_count_; i++)
        {
            iov.get()[i].iov_base = aligned_alloc(pgsize_, buff_size_);
            iov.get()[i].iov_len = buff_size_;
        }
        __s32 read_butch = static_cast<__s32>(buff_size_ * queue_count_);
        std::size_t num_blocks = config_.get_alloc_chunk() / read_butch;

        timerw timer;

        for (std::size_t i = 0; i < num_blocks; i++)
        {
            off_t offset = i * read_butch;

            struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
            if (!sqe)
                raise_err("io_uring_get_sqe failed");

            timer.start();
            sqe->flags |= IOSQE_IO_DRAIN;
            io_uring_prep_readv(sqe, fd_, iov.get(), queue_count_, offset);

            if (io_uring_submit(&ring) < 0)
                raise_err("io_uring_submit failed");

            struct io_uring_cqe* cqe;
            if (io_uring_wait_cqe(&ring, &cqe) < 0)
                raise_err("io_uring_wait_cqe failed");

            if (cqe->res != read_butch)
                raise_err("read failed");

            timer.stop();
            io_uring_cqe_seen(&ring, cqe);
        }

        long micr = timer.nanoseconds();
        timer.reset();
        double elapsed = static_cast<double>(micr)/1e9;

        double throughput = static_cast<double>(config_.get_alloc_chunk()) / elapsed / CNFG_1MG;
        push_msg(job_msg{throughput, measure_type::READ});
    }

//    close(fd_);
    io_uring_queue_exit(&ring);








}


} // jobs