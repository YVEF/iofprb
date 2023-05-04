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

//    std::string error;
//    msg_class msgclass;
};

struct errmsg
{
    std::string_view error;
};

class job
{
public:
    explicit job(const config_state&, const diskctx*) noexcept;
    void start();

    bool pull_msg(job_msg*) noexcept;
    virtual ~job() noexcept = default;

protected:
    virtual void start_() = 0;
    virtual void initialize_() {}
    void push_msg(job_msg) noexcept;


    const config_state& config_;
    const diskctx* disk_;

private:
    std::vector<void(*)(const job_msg&)> subs_;
    std::vector<std::thread> workers_;
    std::queue<job_msg> msgs_;
};

job* initialize_job(const config_state&, const diskctx*) noexcept;


} // job

#endif //IOFPRB_JOB_H
