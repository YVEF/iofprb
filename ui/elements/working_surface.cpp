#include "working_surface.h"
#include "../uihelper.h"
#include <algorithm>
#include "../../utils/debug.h"

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


        if (ImGui::BeginTable("Partitions", 6))
        {
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoSort, 50);
            ImGui::TableSetupColumn("Logic name", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable);
            ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable, 80);
            ImGui::TableSetupColumn("Sectors count", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable);
            ImGui::TableSetupColumn("Partition Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_IndentEnable);
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
                ImGui::Text("%ud", part->sector_cnt);
                ImGui::TableNextColumn();
                ImGui::Text("%s", part->partition_type.c_str());
                ImGui::TableNextColumn();


                ImGui::ProgressBar(static_cast<float>(part->size - part->free)/static_cast<float>(part->size));
            }
            ImGui::EndTable();
        }

        float off = ctx.get_offset().y;
        ImGui::SetWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - off));
        ImGui::SetWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + off));
        ImGui::End();
    }

    ImGui::PopStyleColor();
}

} // ui