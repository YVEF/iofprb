#ifndef IOFPRB_JOB_H
#define IOFPRB_JOB_H
#include <memory>
#include "../providers/config_state.h"
#include <vector>
#include <thread>
#include <queue>


namespace jobs {

enum class measure_type
{
    READ,
    WRITE
};

struct job_msg
{
    // read/write throughput
    double throughput;
    measure_type mtype;
//    double write_thr;
};

class job
{
public:
    explicit job(const config_state&) noexcept;
    void start();

//    void register_notify(void(* callback)(const job_msg&)) noexcept;
    bool pull_msg(job_msg*) noexcept;
    virtual ~job() noexcept = default;

protected:
    virtual void start_() = 0;
    void push_msg(job_msg) noexcept;

    const config_state& config_;

private:
    std::vector<void(*)(const job_msg&)> subs_;
    std::vector<std::thread> workers_;
    std::queue<job_msg> msgs_;
};

job* initialize_job(const config_state& config) noexcept;


} // job

#endif //IOFPRB_JOB_H
