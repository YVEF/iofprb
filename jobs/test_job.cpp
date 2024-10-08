#include "test_job.h"
#include <random>
#include <chrono>
#include <cassert>
#include <thread>

namespace jobs {

std::random_device seed;
std::mt19937 engine(seed());
std::uniform_real_distribution<double> dist(30.f, 50.f);

void test_job::start_()
{
    update_phase("preparing");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    update_phase("running");

    int i = config_.get_iterations();
    while(i-- > 0)
    {
        terminate_if_requested();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        push_msg(job_msg{dist(engine), measure_type::READ});
    }

    i = config_.get_iterations();
    while(i-- > 0)
    {
        terminate_if_requested();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        push_msg(job_msg{dist(engine), measure_type::WRITE});
    }
}

} // jobs