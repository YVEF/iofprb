#ifndef IOFPRB_SYSCALLS_JOB_H
#define IOFPRB_SYSCALLS_JOB_H
#include "job.h"

namespace jobs {

class syscalls_job : public job
{
public:
    explicit syscalls_job(const config_state& config) noexcept : job(config) {}

protected:
    void start_() override;

};

} // jobs

#endif //IOFPRB_SYSCALLS_JOB_H
