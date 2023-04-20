#ifndef IOFPRB_TOPMENU_H
#define IOFPRB_TOPMENU_H
#include "base_element.h"

namespace ui {
class topmenu : public base_element
{
    ImGuiWindowFlags flags_ = ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoSavedSettings;

public:
    void render(render_context& ctx) noexcept override;
};

}


#endif //IOFPRB_TOPMENU_H
