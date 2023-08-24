#include "job.h"
#include "test_job.h"
#include <cassert>
#include "syscalls_job.h"
#include <functional>
#include <memory>
#include <thread>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <csignal>
#include <sys/mman.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>

namespace jobs {

job::job(const config_state& config, const diskctx* disk) noexcept
: config_(config), disk_(disk) {}

void job::start()
{
    job* this_ = this;
    this_->initialize_();
    is_running = true;
    for(uint i=0; i<config_.get_threads(); i++)
        workers_.emplace_back([this_]() { this_->start_(); });
}

void job::terminate_if_requested() const noexcept
{
    if(stop_by_termination.load(std::memory_order_acquire))
        std::terminate();
}

void job::stop()
{
    if(!is_running) return;
    update_phase("Stopping");
    stop_by_termination.store(true, std::memory_order_release);

    stopping_task_ = std::async(std::launch::async, [this](){
        for(auto& w : workers_)
            w.join();

        is_running.store(false, std::memory_order_release);
    });
}

void job::push_msg(jobs::job_msg msg) noexcept
{
    msgs_.push(msg);
}

const char* job::phase() const noexcept
{
    return phase_;
}

void job::update_phase(const char* new_phase) noexcept
{
    phase_ = new_phase;
}

bool job::pull_msg(jobs::job_msg* msg) noexcept
{
    if(msgs_.empty())
        return false;

    *msg = msgs_.back();
    msgs_.pop();
    return true;
}

std::shared_ptr<job> allocate_job(const config_state& config, const diskctx* disk) noexcept
{
#ifdef DUMMY_TEST_ENGINE
    assert(config.engine_id <= 2);
#else
    assert(config.engine_id <= 1);
#endif

    switch (config.get_engine())
    {
        case engine::STRD:
            return std::make_shared<syscalls_job>(config, disk);
        case engine::KMOD:
            assert(false);
#ifdef DUMMY_TEST_ENGINE
        case engine::TEST:
            return std::make_shared<test_job>(config, disk);
#endif
    }

    assert(false);
}

} // job