#include "testmenu.h"
#include <imgui.h>


namespace ui {

int testmenu::next() const noexcept
{
    return 0;
}

void testmenu::render(ui::render_context& ctx) noexcept
{
    auto viewport = ImGui::GetMainViewport();
    if(ImGui::Begin("sdf", nullptr, flags_))
    {
        float off = ctx.get_offset().y;
        ImGui::SetWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - off));
        ImGui::SetWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + off));

        ImGui::End();
    }
}

} // ui