#include "start_menu.h"
#include "../uihelper.h"
#include "../widgets/imgui_customs.h"
#include "../../utils/hlp.h"

namespace ui {

int start_menu::next() const noexcept
{
    if(is_run_pressed_)
        return 1;

    return 0;
}

void start_menu::reset_config() const noexcept
{
    config_.reset_partition();
}

void start_menu::render(ui::render_context& ctx) noexcept
{
    auto viewport = ImGui::GetMainViewport();
    if(ImGui::Begin("##Main menu", nullptr, flags_))
    {
        auto& disks = config_.disks_info;
        if(disks.empty())
        {
            ImGui::End();
            return;
        }

        if(ImGui::Custom::RenderCombo("##diskmenu", -1.f, &config_.disk_info_id, config_.disk_names))
            reset_config();

        ImGui::SameLine(0, ctx.standard_space);

        if(ImGui::Custom::RenderButton("Run", ctx))
            is_run_pressed_ = true;

        const std::string& description = config_.get_disk_description();

        ImGui::Separator();
        ImGui::Text("%s", description.c_str());
        ImGui::NewLine();

        auto& current_disk = disks[config_.disk_info_id];

        ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, ctx.colors.borders_black);
        ImGui::PushStyleColor(ImGuiCol_TableBorderLight, ctx.colors.borders_white2);
        ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ctx.colors.hovered_button_color2);

        if (ImGui::BeginTable("Partitions", 9, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, 50);
            ImGui::TableSetupColumn("Logic Name", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable);
            ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable, 75);
            ImGui::TableSetupColumn("Sectors Count", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable);
            ImGui::TableSetupColumn("Partition Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable);
            ImGui::TableSetupColumn("Mount Point");
            ImGui::TableSetupColumn("FS", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable);
            ImGui::TableSetupColumn("Total/Free", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Usage");

            ImGui::TableHeadersRow();

            for (std::size_t row = 0; row < current_disk.partitions->size(); row++)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::RadioButton(std::string("#" + std::to_string(row)).c_str(), &config_.partition_id, (int)row);

                auto part = current_disk.partitions->at(row);
                ImGui::TableNextColumn();
                ImGui::Text("%s", part->devname.c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%ld", part->offset);
                ImGui::TableNextColumn();
                ImGui::Text("%u", part->sector_cnt);
                ImGui::TableNextColumn();
                ImGui::Text("%s", part->partition_type.c_str());
                ImGui::TableNextColumn();

                if(part->is_swap)
                    ImGui::Text("%s", part->fstype.c_str());
                else if(!part->is_mounted())
                    ImGui::Text("%s", "Not Mounted");
                else
                    ImGui::Text("%s", part->mntroot.c_str());

                ImGui::TableNextColumn();
                ImGui::Text("%s", part->fstype.c_str());
                ImGui::TableNextColumn();

                // free
                if(part->is_swap)
                    ImGui::Text("-/-");
                else if(!part->is_mounted())
                    ImGui::Text("%s/-", to_pretty_size(part->size).c_str());
                else
                    ImGui::Text("%s/%s", to_pretty_size(part->size).c_str(), to_pretty_size(part->free).c_str());
                ImGui::TableNextColumn();

                // usage
                if(!part->is_mounted() || part->is_swap)
                    ImGui::Text("-");
                else
                {
                    ImGui::Custom::ProgressBar(static_cast<float>(part->size - part->free)/static_cast<float>(part->size),
                                               0, 1, "%s", ImVec2(ImGui::GetColumnWidth(), 11),
                                               ctx.colors.brown1,
                                               ctx.colors.gray_brown,
                                               ctx.colors.black);
                }
            }
            ImGui::EndTable();
        }
        ImGui::PopStyleColor(3);
        ImGui::NewLine();

        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.55f);

        if(ImGui::BeginChild("bordered_combo", ImVec2(ImGui::GetContentRegionAvail().x, 35), true))
        {
            ImGui::Custom::RenderCombo("Allocations", 75.f, &config_.alloc_chnk_id, config_.alloc_chunks);

            ImGui::SameLine(0, ctx.standard_space);
            ImGui::Custom::RenderCombo("Iterations", 45.f, &config_.iterations_id, config_.iterations);

            ImGui::SameLine(0, ctx.standard_space);
            ImGui::Custom::RenderCombo("Threads", 45.f, &config_.threads_id, config_.threads, false);

            ImGui::SameLine(0, ctx.standard_space);
            ImGui::Custom::RenderCombo("Access Type", 51.f, &config_.access_id, config_.access, false);

            ImGui::SameLine(0, ctx.standard_space);
            ImGui::Custom::RenderCombo("Blk Size", 61.f, &config_.block_size_id, config_.block_size);

            ImGui::SameLine(0, ctx.standard_space);
            ImGui::Custom::RenderCombo("Engine", 90.f, &config_.engine_id, config_.engines);

            ImGui::EndChild();
        }

        if(ImGui::BeginChild("tune", ImVec2(ImGui::GetContentRegionAvail().x, 35), true))
        {
            ImGui::Checkbox("Preserve History", &config_.preserve_history);
            ImGui::SameLine(0, ctx.standard_space);

            if(!config_.preserve_history) ImGui::Custom::PushDisabledStyle();
            ImGui::InputText("history directory", config_.hist_dir, CNFG_HIST_DIR_LEN);
            if(!config_.preserve_history) ImGui::Custom::PullDisabledStyle();

            ImGui::EndChild();
        }

        ImGui::PopStyleVar();

        float off = ctx.get_offset().y;
        ImGui::SetWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - off));
        ImGui::SetWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + off));
        ImGui::End();
    }
}


} // ui