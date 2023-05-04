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
    int a = 0;
    int i = config_.get_iterations();
    while(true)
    {
        auto mtype = (a / i) % 2 == 0 ?
                measure_type::READ : measure_type::WRITE;

        std::this_thread::sleep_for(std::chrono::seconds(1));
        push_msg(job_msg{dist(engine), mtype});
        a++;
    }
}

} // jobs