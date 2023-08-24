#ifndef IOFPRB_START_MENU_H
#define IOFPRB_START_MENU_H
#define IMGUI_DEFINE_MATH_OPERATORS

#include "nextback_canvas.h"
#include "../../ambient/config_state.h"


namespace ui {

class start_menu : public nextback_canvas
{
public:
    BASE_EL_REGULAR_OVERRIDE
    NB_CANV_REGULAR_OVERRIDE

    explicit start_menu(config_state& config, const ambient::driveprv&) noexcept
    : config_(config)
    {

    }

private:
    ImGuiWindowFlags flags_ = ImGuiWindowFlags_NoTitleBar
                              | ImGuiWindowFlags_NoResize
                              | ImGuiWindowFlags_NoMove
                              | ImGuiWindowFlags_NoScrollbar
                              | ImGuiWindowFlags_NoSavedSettings;

    bool is_run_pressed = false;
    config_state& config_;
};

} // ui

#endif //IOFPRB_START_MENU_H
