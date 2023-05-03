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


template<typename TEll, typename TStr>
void RenderCombo(const char* name, float width, int* selected_item,
                 const std::vector<TEll>& collection,
                 const std::function<TStr(int)>& get_str,
                 const std::function<void(void)>* notify_changed = nullptr)
{
    if(width != -1)
        ImGui::PushItemWidth(width);

    if(ImGui::BeginCombo(name, get_str(*selected_item).data()))
    {
        int sz = static_cast<int>(collection.size());
        for(int i=0; i<sz; i++)
        {
            bool is_sel = *selected_item == i;
            if (ImGui::Selectable(get_str(i).data(), is_sel))
            {
                int prev = *selected_item;
                *selected_item = i;
                if(prev != *selected_item && notify_changed != nullptr)
                    (*notify_changed)();
            }

            if (is_sel)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
    if(width != -1)
        ImGui::PopItemWidth();
}

bool RenderButton(const char* label, const ui::render_context& ctx, bool enabled = true);

}


#endif //IOFPRB_IMGUI_CUSTOMS_H
