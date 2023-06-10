#ifndef IOFPRB_TIMERW_H
#define IOFPRB_TIMERW_H
#include <chrono>
#include "../utils/typedefs.h"
#include <vector>


namespace jobs {

using timepoint_t = std::chrono::time_point<std::chrono::steady_clock>;
class timerw
{
public:
    inline void start() noexcept
    {
        start_tp_ = std::chrono::steady_clock::now();
        is_running_ = true;
    }

    inline void stop() noexcept
    {
        if(!is_running_)
            return;

        auto stop = std::chrono::steady_clock::now();
        elapsed_ += (stop - start_tp_);
    }

    inline void reset() noexcept
    {
        elapsed_ = std::chrono::nanoseconds(0);
    }

    void initialize() noexcept
    {
        start();
        stop();
        reset();
    }

    long microseconds() const noexcept;
    long nanoseconds() const noexcept;

private:
    bool is_running_ = false;
    timepoint_t start_tp_;
    std::chrono::nanoseconds elapsed_{0};
};

} // jobs

#endif //IOFPRB_TIMERW_H
