#include "timerw.h"
#include <chrono>

namespace jobs {

long timerw::microseconds() const noexcept
{
    return std::chrono::duration_cast<std::chrono::microseconds>(elapsed_).count();
}

long timerw::nanoseconds() const noexcept
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_).count();
}

} // jobs