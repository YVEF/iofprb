#include "test_job.h"
#include <random>
#include <chrono>
#include <cassert>

namespace jobs {

std::random_device seed;
std::mt19937 engine(seed());
std::uniform_real_distribution<double> dist(30.f, 50.f);

void test_job::start_()
{
    int i = config_.get_iterations();
    while(i-- > 0)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        push_msg(job_msg{dist(engine), measure_type::READ});
    }

    i = config_.get_iterations();
    while(i-- > 0)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        push_msg(job_msg{dist(engine), measure_type::WRITE});
    }
}

} // jobs