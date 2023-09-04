#include "job.h"
#include "test_job.h"
#include <cassert>
#include "syscalls_job.h"
#include <memory>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <csignal>
#include <sys/mman.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <chrono>

namespace jobs {

#define STACK_2M (1024*1024*2)

job::job(const config_state& config, const diskctx* disk) noexcept
: config_(config), disk_(disk) {}


static int job_fork_proc_fnc(void* args)
{
    auto transit = static_cast<job_transit*>(args);
    transit->start();
    while(transit->jb->is_running.load(std::memory_order_acquire)) sleep(2);
    return 0;
}

job::~job() noexcept
{
    auto ft = stop();
    if(ft.valid()) ft.get();
    delete transit_;
}

void job::start()
{
    child_stack_ = static_cast<char*>(malloc(STACK_2M * sizeof(uint8_t)));
    assert(child_stack_);
    transit_ = new job_transit(this);
    child_pid_ = clone(job_fork_proc_fnc, child_stack_ + STACK_2M, CLONE_VM | SIGCHLD, transit_);
    assert(child_pid_ != -1);
}

void job::terminate_if_requested() const noexcept
{
    if(stop_by_termination.load(std::memory_order_acquire))
        std::terminate();
}

std::future<void> job::stop()
{
//    blkio_delay_total
    if(!is_running.load(std::memory_order_acquire))
        return std::future<void>{};

    update_phase("Stop");
    stop_by_termination.store(true, std::memory_order_release);
    // don't care about possible dangling pointer because the job will
    // be terminated in .dtor in any case
    return std::async(std::launch::async, [this]() noexcept {
        try
        {
            auto workers_joining = std::async([&workers = workers_](){
                for(auto& w : workers)
                    w.join();
            });
            std::cout << "!!!!!11" << std::endl;
            workers_joining.wait_for(std::chrono::seconds(30));
            std::cout << "here" << std::endl;
            auto rr = waitpid(child_pid_, nullptr, WNOHANG);
            if(rr != -1)
            {
                std::cout << "terminated by SIGINT" << "r = " << rr<< std::endl;
                kill(child_pid_, SIGINT);
            }
        }
        catch(...) {}
        free(child_stack_);
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

void job_transit::start()
{
    jb->initialize_();
    jb->is_running.store(true, std::memory_order_release);
    for(uint i=0; i<jb->config_.get_threads(); i++)
        jb->workers_.emplace_back([this]() { jb->start_(); });

    jb->ending_future_ = std::async(std::launch::async, [this](){
        for(auto& w : jb->workers_)
            w.join();

        jb->is_running.store(false, std::memory_order_release);
    });
}

} // job