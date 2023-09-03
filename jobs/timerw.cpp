#include "timerw.h"
#include <chrono>

namespace jobs {

namespace internal {
#define TIMERW_SYSCALS_NMB 16000U
#define read_barrier()	__asm__ __volatile__("":::"memory")
#define X64_READV	 19
constexpr static int nullfd = -1;		/* File descriptor for /dev/null */
static long long probe_syscall(int msb, int lsb)
{
    register long long arg1 asm("rdi") = nullfd;
    register long long arg2 asm("rsi") = 0;
    register long long arg3 asm("rdx") = 0;
    register long long arg4 asm("r10") = 0;
    register long long arg5 asm("r8")  = 0;
    register long long arg6 asm("r9")  = 0;
    long long nr = ((long long)msb << 32) | (unsigned int)lsb;
    long long ret;

    /*
     * We pass in an extra copy of the extended system call number
     * in %rbx, so we can examine it from the ptrace handler without
     * worrying about it being possibly modified. This is to test
     * the validity of struct user regs.orig_rax a.k.a.
     * struct pt_regs.orig_ax.
     */
    asm volatile("syscall"
            : "=a" (ret)
            : "a" (nr), "b" (nr),
    "r" (arg1), "r" (arg2), "r" (arg3),
    "r" (arg4), "r" (arg5), "r" (arg6)
            : "rcx", "r11", "memory", "cc");

    return ret;
}

}

std::chrono::nanoseconds timerw::elapsed(std::size_t syscalls_count) const noexcept
{
    return elapsed_ - syscall_cost_ * syscalls_count;
}

long timerw::microseconds(std::size_t syscalls_count) const noexcept
{
    return std::chrono::duration_cast<std::chrono::microseconds>(elapsed(syscalls_count)).count();
}

long timerw::nanoseconds(std::size_t syscalls_count) const noexcept
{
    return elapsed(syscalls_count).count();
}

void timerw::start() noexcept
{
    start_tp_ = std::chrono::steady_clock::now();
    is_running_ = true;
}

void timerw::stop() noexcept
{
    if(!is_running_)
        return;

    auto stop = std::chrono::steady_clock::now();
    elapsed_ += (stop - start_tp_);
}

void timerw::reset() noexcept
{
    elapsed_ = std::chrono::nanoseconds(0);
}

void timerw::update_syscalls_cost() noexcept
{
    syscall_cost_ = std::chrono::nanoseconds{0};
    start();
    for(std::size_t i=0; i<TIMERW_SYSCALS_NMB; i++)
    {
        read_barrier();
        auto r = internal::probe_syscall(0, X64_READV);
    }
    stop();
    syscall_cost_ = elapsed_ / TIMERW_SYSCALS_NMB;
    reset();
}

void timerw::initialize() noexcept
{
    start();
    stop();
    reset();
    update_syscalls_cost();
}

} // jobs