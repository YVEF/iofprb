#ifndef IOFPRB_BENCH_PROCESS_H
#define IOFPRB_BENCH_PROCESS_H
#include "nextback_canvas.h"
#include "../../jobs/job.h"
#include <implot.h>

namespace ui {

class bench_chart : public nextback_canvas
{
public:
    NB_CANV_REGULAR_OVERRIDE
    BASE_EL_REGULAR_OVERRIDE

    explicit bench_chart(config_state&, const providers::driveprv&) noexcept;
    ~bench_chart() noexcept override;

private:
    jobs::job* job_;
    config_state& config_;
    bool goback;
    bool was_stopped_;
    uint c_iteration_;
    uint c_round_;
    double read_max_;
    double read_min_;
    double write_max_;
    double write_min_;
    int round_switch_;
    const providers::driveprv& driverprv_;

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
