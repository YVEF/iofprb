#ifndef IOFPRB_JOB_H
#define IOFPRB_JOB_H
#include <memory>
#include "../ambient/config_state.h"
#include <vector>
#include <thread>
#include <queue>
#include <future>

namespace jobs {

enum class measure_type
{
    READ,
    WRITE
};

enum class msg_class
{
    REPORT,
    ERR
};

struct job_msg
{
    // read/write throughput
    double throughput;
    measure_type mtype;
};

struct job_transit;

class job
{
    friend struct job_transit;
public:
    explicit job(const config_state&, const diskctx*) noexcept;
    void start();
    std::future<void> stop();

    bool pull_msg(job_msg*) noexcept;
    virtual const char* phase() const noexcept;

    virtual ~job() noexcept;
    std::atomic<bool> is_running;


protected:
    virtual void start_() = 0;
    virtual void initialize_() {}
    void push_msg(job_msg) noexcept;
    void update_phase(const char* new_phase) noexcept;

    const config_state& config_;
    const diskctx* disk_;
    void terminate_if_requested() const noexcept;

private:
    std::vector<std::thread> workers_;
    std::queue<job_msg> msgs_;
    const char* phase_ = "-/-";
    std::atomic<bool> stop_by_termination = false;
    std::future<void> terminating_future_;
    std::future<void> ending_future_;

    int child_pid_;
    char* child_stack_;
    job_transit* transit_;
};

std::shared_ptr<job> allocate_job(const config_state&, const diskctx*) noexcept;

/*
 * job transiction to child process
 */
struct job_transit
{
    explicit job_transit(job* j) noexcept : jb(j) {}
    void start();
    // do not free the pointer memory,
    // because the class is not an owner of the job
    job* jb;
};

} // job

#endif //IOFPRB_JOB_H
