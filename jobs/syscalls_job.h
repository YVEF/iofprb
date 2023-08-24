#ifndef IOFPRB_SYSCALLS_JOB_H
#define IOFPRB_SYSCALLS_JOB_H
#include "job.h"
#include <liburing.h>

#define SYSJOB_DEFAULT_BUF_SZ 16384U // 16Kb

namespace jobs {

class syscalls_job : public job
{
public:
    explicit syscalls_job(const config_state&, const diskctx*) noexcept;
    ~syscalls_job() noexcept override;

protected:
    void start_() override;
    void initialize_() override;


private:
    int fd_ = 0;
    long pgsize_;
    uint buff_size_;
    uint queue_count_;

    static void raise_err(const char* errmsg);
    std::string mapping_file_;
    std::string mapping_dir_;
    partinfo* device_partition_;

    using io_uring_prev_reqv_t = void(*)(struct io_uring_sqe *sqe, int fd,
                                         const struct iovec *iovecs,
                                         unsigned nr_vecs, __u64 offset);

    double emit_requests(const std::shared_ptr<struct iovec>& iov,
                         std::size_t num_blocks,
                         struct io_uring& ring,
                         __s32 butch_size,
                         io_uring_prev_reqv_t);

};

} // jobs

#endif //IOFPRB_SYSCALLS_JOB_H
