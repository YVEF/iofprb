#ifndef IOFPRB_BENCH_PROCESS_H
#define IOFPRB_BENCH_PROCESS_H
#include "nextback_canvas.h"
#include "../../jobs/job.h"
#include "../../ambient/hist.h"
#include <implot.h>

namespace ui {

class bench_chart : public nextback_canvas
{
public:
    NB_CANV_REGULAR_OVERRIDE
    BASE_EL_REGULAR_OVERRIDE
    void adjust() noexcept override;

    explicit bench_chart(config_state&, const ambient::driveprv&) noexcept;
    ~bench_chart() noexcept override;

private:
    std::shared_ptr<jobs::job> job_;
    std::unique_ptr<ambient::hist> hist_;
    config_state& config_;
    bool goback;
    bool was_stopped_;
    bool stop_requtested_;
    uint c_iteration_;
    uint c_round_;
    double read_max_;
    double read_min_;
    double write_max_;
    double write_min_;
    const ambient::driveprv& driverprv_;
    std::future<void> job_terminating_;

//    int child_pid_;
//    char* child_stack_;



    // read/write reports
    std::vector<double> iterations_;
    std::vector<double> round_read_thr_;
    std::vector<double> round_write_thr_;

    ImGuiWindowFlags blockflags_ = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                                   | ImGuiWindowFlags_NoSavedSettings
                                   | ImGuiWindowFlags_NoResize
                                   | ImGuiWindowFlags_NoMove;

    ImPlotFlags plotflags_ = ImPlotFlags_NoLegend | ImPlotFlags_NoFrame
                      | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText
                      | ImPlotFlags_NoMenus;


};

} // ui

#endif //IOFPRB_BENCH_PROCESS_H
