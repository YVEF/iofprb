#ifndef IOFPRB_SYSCALLS_JOB_H
#define IOFPRB_SYSCALLS_JOB_H
#include "job.h"

#define SYSJOB_DEFAULT_BUF_SZ 16384U // 16Kb

namespace jobs {

class syscalls_job : public job
{
public:
    explicit syscalls_job(const config_state& config, const diskctx* disk) noexcept
    : job(config, disk) {}

protected:
    void start_() override;
    void initialize_() override;
    ~syscalls_job() noexcept override;

private:
    int fd_ = 0;
    long pgsize_;
    uint buff_size_;
    uint queue_count_;

    static void push_err(const char* errmsg);

};

} // jobs

#endif //IOFPRB_SYSCALLS_JOB_H
