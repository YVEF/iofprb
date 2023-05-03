#include "topmenu.h"
#include <cstdlib>

namespace ui {

void topmenu::render(render_context& ctx) noexcept
{
    bool show_popup = false;
    bool close_app = false;

    ImGui::BeginMainMenuBar();
    ctx.add_offset(ImVec2(0, ImGui::GetWindowSize().y));

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_MenuBarBg] = ctx.colors.main_color_dark1;
    style.Colors[ImGuiCol_Text] = ctx.colors.active_text;
    style.Colors[ImGuiCol_HeaderHovered] = ctx.colors.hovered_button_color1;

    if (ImGui::BeginMenu("File"))
    {
        ImGui::MenuItem("Settings");
        if(ImGui::MenuItem("Exit"))
            close_app = true;

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help"))
    {
        if(ImGui::MenuItem("About"))
            show_popup = true;

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    if(show_popup)
        ImGui::OpenPopup("About");

    if(ImGui::BeginPopupModal("About", nullptr, flags_))
    {
        ImGui::Text("This is my about popup window!");
        if (ImGui::Button("OK"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    if (show_popup)
        ImGui::BeginDisabled();
    if (show_popup)
        ImGui::EndDisabled();

    if(close_app)
    {
        exit(EXIT_SUCCESS);
    }
}

}

