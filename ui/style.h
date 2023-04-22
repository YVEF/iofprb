#ifndef IOFPRB_STYLE_H
#define IOFPRB_STYLE_H
#include <imgui.h>

namespace ui {
struct style
{
    const ImVec4 main_color_dark1 = ImVec4(0.21f, 0.21f, 0.21f, 1.0f);
    const ImVec4 main_color_dark2 = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
    const ImVec4 hovered_button_color1 = ImVec4(0.3f, 0.3f, 0.7f, 0.7f);
    const ImVec4 hovered_button_color2 = ImVec4(0.3f, 0.3f, 0.50f, 0.7f);
    const ImVec4 active_text = ImVec4(1.0f, 1.0f, 1.0f, 0.92f);
    const ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    const ImVec4 borders_white1 = ImVec4(0.85f, 0.85f, 0.85f, 0.3f);
    const ImVec4 borders_white2 = ImVec4(0.75f, 0.75f, 0.75f, 0.27f);
    const ImVec4 borders_black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    const ImVec4 brown1 = ImVec4(0.45f, 0.32f, 0.22f, 0.94f);
    const ImVec4 black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    const ImVec4 gray_brown = ImVec4(0.38f, 0.35f, 0.30f, 0.98f);
    const ImVec4 gray_brown2 = ImVec4(0.36f, 0.34f, 0.32f, 0.83f);
};

}

#endif //IOFPRB_STYLE_H
