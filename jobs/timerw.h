#ifndef IOFPRB_TIMERW_H
#define IOFPRB_TIMERW_H
#include <chrono>
#include "../utils/typedefs.h"

namespace jobs {

using timepoint_t = std::chrono::time_point<std::chrono::steady_clock>;
class timerw
{

public:
    inline void start() noexcept
    {
        start_tp_ = std::chrono::steady_clock::now();
    }

    inline void stop() noexcept
    {
        stop_tp_ = std::chrono::steady_clock::now();
        elapsed_ = stop_tp_ - start_tp_;
    }

    long microseconds() const noexcept;
    long nanoseconds() const noexcept;

private:
    timepoint_t start_tp_;
    timepoint_t stop_tp_;
    decltype(stop_tp_ - start_tp_) elapsed_;


};

} // jobs

#endif //IOFPRB_TIMERW_H
