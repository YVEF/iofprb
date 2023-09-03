#ifndef IOFPRB_IMGUI_CUSTOMS_H
#define IOFPRB_IMGUI_CUSTOMS_H

#define IMGUI_DEFINE_MATH_OPERATORS

#include <functional>
#include <imgui.h>
#include <imgui_internal.h>
#include "../render_context.h"
namespace ImGui::Custom {

float ProgressBar(float value, const float minValue, const float maxValue, const char *format,
                  const ImVec2 &sizeOfBarWithoutTextInPixels, const ImVec4 &colorLeft, const ImVec4 &colorRight, const ImVec4 &colorBorder);

void PushDisabledStyle() noexcept;
void PullDisabledStyle() noexcept;

template<typename T1>
bool RenderCombo(const char* name, float width,
                 int* selected_item,
                 const config_state::valuelist_pairs_t<T1>& values,
                 bool enabled = true)
{
    bool was_changed = false;
    if(width != -1) ImGui::PushItemWidth(width);
    if(!enabled) PushDisabledStyle();

    if(ImGui::BeginCombo(name, values[*selected_item].second.data()))
    {
        int sz = static_cast<int>(values.size());
        for(int i=0; i<sz; i++)
        {
            bool is_sel = *selected_item == i;
            if (ImGui::Selectable(values[i].second.data(), is_sel))
            {
                int prev = *selected_item;
                *selected_item = i;
                if(prev != *selected_item) was_changed = true;
            }

            if (is_sel)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
    if(width != -1) ImGui::PopItemWidth();
    if(!enabled) PullDisabledStyle();

    return was_changed;
}

bool RenderButton(const char* label, const ui::render_context& ctx, bool enabled = true);

inline void RenderFancyLoader()
{
    ImGui::Text("%c", "|/-\\"[(int)(ImGui::GetTime() / 0.1f) & 3]);
}

}




#endif //IOFPRB_IMGUI_CUSTOMS_H
