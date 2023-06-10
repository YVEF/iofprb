#define IMGUI_DEFINE_MATH_OPERATORS

#include "bench_chart.h"
#include <implot.h>
#include <algorithm>
#include "../widgets/imgui_customs.h"

namespace ui {

static inline double update_min(double min, double max, const jobs::job_msg& msg, jobs::measure_type measure) noexcept
{
    return min == -1.0 ?
          max :
          msg.mtype == measure ?
            std::min(min, msg.throughput) :
            min;
}

int bench_chart::next() const noexcept
{
    return goback ? -1 : 0;
}

void bench_chart::adjust() noexcept
{
    job_->start();
}

bench_chart::bench_chart(config_state& config, const providers::driveprv& drv) noexcept
: config_(config), goback(false), was_stopped_(false), c_iteration_(0), c_round_(1),
    read_max_(0.0), read_min_(-1.0), write_max_(0.0), write_min_(-1.0), round_switch_(0),
    driverprv_(drv)
{
    iterations_.push_back(0.0);
    round_read_thr_.push_back(0.0);
    round_write_thr_.push_back(0.0);
    job_ = jobs::initialize_job(config_, driverprv_.get_disk(config_.get_disk_uuid()));
}

void bench_chart::render(ui::render_context& ctx) noexcept
{
    auto viewport = ImGui::GetMainViewport();

    // brief iteration description
    if(ImGui::Begin("##description", nullptr, blockflags_))
    {
        auto off = ctx.get_offset();
        ImGui::SetWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + off.y));
        ImGui::SetWindowSize(ImVec2(viewport->Size.x, 55));
        ctx.add_offset(ImVec2(0, ImGui::GetWindowHeight()));

        bool is_pop = false;
        if(ImGui::Custom::RenderButton("Back", ctx, was_stopped_))
            goback = true;

        ImGui::SameLine();
        if(ImGui::Custom::RenderButton("Stop", ctx, !was_stopped_))
            was_stopped_ = true;


        ImGui::SameLine();
        if(ImGui::Custom::RenderButton("Clear History", ctx))
            is_pop = true;

        ImGui::SameLine(0, ctx.standard_space);
        ImGui::Text("Disk: %s", config_.disks_info[config_.disk_info_id].name.c_str());

        ImGui::GetColumnOffset();
        ImGui::SameLine(0.0f, ctx.standard_space);
        ImGui::Text("Phase: %s...", job_->phase());
        ImGui::SameLine(0.0f, ctx.standard_space);
        ImGui::Text("%c", "|/-\\"[(int)(ImGui::GetTime() / 0.1f) & 3]);

        if(is_pop)
            ImGui::OpenPopup("clrhist_popup");

        if(ImGui::BeginPopupModal("clrhist_popup", nullptr, blockflags_))
        {
            ImGui::Text("Delete the entire history file? (.iofprbhist)");
            ImGui::NewLine();
            bool ok = ImGui::Button("OK");
            ImGui::SameLine();
            bool discard = ImGui::Button("Discard");
            if (ok || discard)
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }

        ImGui::Text("Round: %u / Iteration %u", c_round_, c_iteration_);
        ImGui::SameLine(0, ctx.standard_space);
        ImGui::Text("Max Read / Write: %lf MiB / %lf MiB", read_max_, write_max_);

        ImGui::End();
    }

    uint iteration_count = config_.get_iterations();
    jobs::job_msg msg{};
    if(job_->pull_msg(&msg))
    {
        c_iteration_++;
        auto& set = msg.mtype == jobs::measure_type::READ ? round_read_thr_ : round_write_thr_;

        if(c_iteration_ - 1 == iteration_count)
        {
            iterations_.clear();
            iterations_.push_back(0.0);
            c_iteration_ = 0;

//            round_switch_++;
//            int switch_disable_idx = (round_switch_ % 2);
//            c_round_ = c_round_ + 1 - switch_disable_idx;
//            if(switch_disable_idx == 0)
//                set.emplace_back();
        }

        iterations_.push_back(msg.throughput);
        double& total = set.back();
        total = std::max(total, msg.throughput);

        // mins for axis
        read_max_ = round_read_thr_.back();
        write_max_ = round_write_thr_.back();

        read_min_ = update_min(read_min_, read_max_, msg, jobs::measure_type::READ);
        write_min_ = update_min(write_min_, write_max_, msg, jobs::measure_type::WRITE);
    }

    if(ImGui::Begin("##chart1", nullptr, blockflags_))
    {
        auto off = ctx.get_offset();
        auto size = ImVec2(viewport->Size.x, 170);
        ImGui::SetWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + off.y));
        ImGui::SetWindowSize(size);
        ctx.add_offset(ImVec2(0, ImGui::GetWindowHeight()));

        ImPlot::SetNextAxisToFit(ImAxis_Y1);
        ImPlot::SetNextAxisLimits(ImAxis_X1, 0, iteration_count, ImPlotCond_Always);

        if (ImPlot::BeginPlot("##lineplot", ImVec2(size.x-17, size.y-17), plotflags_))
        {
            ImPlot::SetupAxes("Iterations", "Throughtput", ImPlotAxisFlags_NoHighlight, ImPlotAxisFlags_NoHighlight);
            ImPlot::PushStyleVar(ImPlotStyleVar_MinorTickLen, ImVec2(1.0f, 0.1f));
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 4.0f);
            ImPlot::PushStyleColor(ImPlotCol_Fill, ctx.colors.shaded_line);
            ImPlot::PushStyleColor(ImPlotCol_Line, ctx.colors.line);

            ImPlot::PlotLine("##line", iterations_.data(), static_cast<int>(iterations_.size()), 1, 0, ImPlotLineFlags_Shaded);
            ImPlot::PopStyleColor(2);
            ImPlot::PopStyleVar(2);
            ImPlot::EndPlot();
        }

        ImGui::End();
    }

    if(ImGui::Begin("##chart2", nullptr, blockflags_))
    {
        auto off = ctx.get_offset();
        auto size = ImVec2(viewport->Size.x, viewport->Size.y - off.y);
        ImGui::SetWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + off.y));
        ImGui::SetWindowSize(size);
        ctx.add_offset(ImVec2(0, ImGui::GetWindowHeight()));

        ImPlot::PushStyleVar(ImPlotStyleVar_PlotBorderSize, 2.f);

        ImPlot::SetNextAxisLimits(ImAxis_Y1, std::min(read_min_, write_min_) - 0.8,
                                  std::max(round_read_thr_.back(), round_write_thr_.back()) + 1.2,
                                  ImPlotCond_Always);

        ImPlot::SetNextAxisLimits(ImAxis_X1, 0, std::max(5, static_cast<int>(round_read_thr_.size() + 1)), ImPlotCond_Always);

        if (ImPlot::BeginPlot("##barplot", ImVec2(size.x-17, size.y-17), plotflags_ & ~ImPlotFlags_NoLegend))
        {
            ImPlot::SetupAxes("Rounds", "Throughtput", ImPlotAxisFlags_NoHighlight, ImPlotAxisFlags_NoHighlight);
            ImPlot::PushStyleColor(ImPlotCol_Fill, ctx.colors.bars1);
            ImPlot::PlotBars("RD", round_read_thr_.data(), static_cast<int>(round_read_thr_.size()), 0.3, 0.15);
            ImPlot::PopStyleColor();

            ImPlot::PushStyleColor(ImPlotCol_Fill, ctx.colors.bars2);
            ImPlot::PlotBars("WR", round_write_thr_.data(), static_cast<int>(round_write_thr_.size()), 0.3, 0.45);
            ImPlot::PopStyleColor();



            ImPlot::EndPlot();
        }

        ImPlot::PopStyleVar();

        ImGui::End();
    }
}


bench_chart::~bench_chart() noexcept
{
    delete job_;
}

} // ui