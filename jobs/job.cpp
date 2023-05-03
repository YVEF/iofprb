#include "job.h"
#include "../providers/config_state.h"
#include "test_job.h"
#include <cassert>
#include "syscalls_job.h"
#include <functional>

namespace jobs {

job::job(const config_state& config) noexcept : config_(config) {}

void job::start()
{
    job* this_ = this;
//    std::thread t([this_]() { this_->start_(); });

    workers_.emplace_back([this_]() { this_->start_(); });

}


void job::push_msg(jobs::job_msg msg) noexcept
{
    msgs_.push(msg);
}

bool job::pull_msg(jobs::job_msg* msg) noexcept
{
    if(msgs_.empty())
        return false;

    *msg = msgs_.back();
    msgs_.pop();
    return true;
}



job* initialize_job(const config_state& config) noexcept
{
#ifdef DUMMY_TEST_ENGINE
    assert(config.engine_id <= 2);
#else
    assert(config.engine_id <= 1);
#endif

    switch (config.engines[config.engine_id].first)
    {
        case engine::STRD:
            return new syscalls_job(config);
        case engine::AHCI:
            assert(false);
        case engine::TEST:
            return new test_job(config);
    }

    assert(false);
}

} // job