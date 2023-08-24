#ifndef IOFPRB_TEST_JOB_H
#define IOFPRB_TEST_JOB_H
#include "job.h"


namespace jobs {

class test_job : public job
{
public:
    explicit test_job(const config_state& config, const diskctx* disk) noexcept
    : job(config, disk) {}

protected:
    void start_() override;

};

} // jobs

#endif //IOFPRB_TEST_JOB_H
