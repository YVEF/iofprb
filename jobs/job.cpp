#include "job.h"
#include "test_job.h"
#include <cassert>
#include "syscalls_job.h"
#include <functional>

namespace jobs {

job::job(const config_state& config, const diskctx* disk) noexcept
: config_(config), disk_(disk) {}

void job::start()
{
    job* this_ = this;
    this_->initialize_();

    for(uint i=0; i<config_.get_threads(); i++)
        workers_.emplace_back([this_]() { this_->start_(); });
}


void job::push_msg(jobs::job_msg msg) noexcept
{
    msgs_.push(std::move(msg));
}

bool job::pull_msg(jobs::job_msg* msg) noexcept
{
    if(msgs_.empty())
        return false;

    *msg = msgs_.back();
    msgs_.pop();
    return true;
}

job* initialize_job(const config_state& config, const diskctx* disk) noexcept
{
#ifdef DUMMY_TEST_ENGINE
    assert(config.engine_id <= 2);
#else
    assert(config.engine_id <= 1);
#endif

    switch (config.get_engine())
    {
        case engine::STRD:
            return new syscalls_job(config, disk);
        case engine::AHCI:
            assert(false);
        case engine::TEST:
            return new test_job(config, disk);
    }

    assert(false);
}

} // job