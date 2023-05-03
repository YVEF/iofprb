#ifndef IOFPRB_TEST_JOB_H
#define IOFPRB_TEST_JOB_H
#include "job.h"


namespace jobs {

class test_job : public job
{
public:
    explicit test_job(const config_state& config) noexcept : job(config) {}

protected:
    void start_() override;

};

} // jobs

#endif //IOFPRB_TEST_JOB_H
