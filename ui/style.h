#ifndef IOFPRB_STYLE_H
#define IOFPRB_STYLE_H
#include <imgui.h>

namespace ui {
struct style
{
    const ImVec4 main_color_dark1 = ImVec4(0.21f, 0.21f, 0.21f, 1.0f);
    const ImVec4 main_color_dark2 = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
    const ImVec4 hovered_button_color1 = ImVec4(0.3f, 0.3f, 0.7f, 0.7f);
    const ImVec4 active_text = ImVec4(1.0f, 1.0f, 1.0f, 0.92f);
    const ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
};

}

#endif //IOFPRB_STYLE_H
