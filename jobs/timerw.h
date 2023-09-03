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
    void start() noexcept;
    void stop() noexcept;
    void reset() noexcept;
    void initialize() noexcept;
    void update_syscalls_cost() noexcept;
    long microseconds(std::size_t syscalls_count) const noexcept;
    long nanoseconds(std::size_t syscalls_count) const noexcept;

private:
    bool is_running_ = false;
    timepoint_t start_tp_;
    std::chrono::nanoseconds elapsed_{0};
    std::chrono::nanoseconds syscall_cost_{0};
    std::chrono::nanoseconds elapsed(std::size_t syscalls_count) const noexcept;
};

} // jobs

#endif //IOFPRB_TIMERW_H
