#include "working_surface.h"
#include "../uihelper.h"
#include <algorithm>
#include <imgui_internal.h>
#include "../../utils/debug.h"
#include "../widgets/misc.h"
#include "../../utils/hlp.h"


namespace ui {


void working_surface::render(render_context& ctx) noexcept
{
    auto viewport = ImGui::GetMainViewport();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ctx.styles.main_color_dark2);
    if(ImGui::Begin("##Main menu", nullptr, flags_))
    {
        static const char* selected_disk = diskinfos_.begin() != diskinfos_.end() ?
                                           diskinfos_[0].name.c_str()
                                                                                  : nullptr;

        static const char* text = diskinfos_.begin() != diskinfos_.end() ?
                  diskinfos_[0].description.c_str()
                 : "";

        if(ImGui::BeginCombo("##testcombo", selected_disk))
        {
            for(const auto& info : diskinfos_)
            {
                bool is_selected = (selected_disk == info.name.c_str());
                if (ImGui::Selectable(info.name.c_str(), is_selected))
                {
                    selected_disk = info.name.c_str();
                    text = info.description.c_str();
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        ImGui::Separator();
        ImGui::Text("%s", text);
        ImGui::NewLine();
        auto curr_disk = std::find_if(diskinfos_.begin(), diskinfos_.end(), [](const auto& d) { return d.name == selected_disk; });
        ASSERT(curr_disk != diskinfos_.end());


        ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, ctx.styles.borders_black);
        ImGui::PushStyleColor(ImGuiCol_TableBorderLight, ctx.styles.borders_white2);
        ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ctx.styles.hovered_button_color2);

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
            for (std::size_t row = 0; row < curr_disk->partitions->size(); row++)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::RadioButton("btn", false);

                auto part = curr_disk.base()->partitions->at(row);
                ImGui::TableNextColumn();
                ImGui::Text("%s", part->name.c_str());
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
                                               ctx.styles.brown1,
                                               ctx.styles.gray_brown,
                                               ctx.styles.black);
                }
            }
            ImGui::EndTable();
        }
        ImGui::PopStyleColor(3);

        float off = ctx.get_offset().y;
        ImGui::SetWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - off));
        ImGui::SetWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + off));
        ImGui::End();
    }

    ImGui::PopStyleColor();
}

} // ui