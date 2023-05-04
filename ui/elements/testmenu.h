#ifndef IOFPRB_TESTMENU_H
#define IOFPRB_TESTMENU_H
#include "nextback_canvas.h"

namespace ui {

class testmenu : public nextback_canvas
{
public:
    BASE_EL_REGULAR_OVERRIDE
    NB_CANV_REGULAR_OVERRIDE

private:
    ImGuiWindowFlags flags_ = ImGuiWindowFlags_NoTitleBar
                              | ImGuiWindowFlags_NoResize
                              | ImGuiWindowFlags_NoMove
                              | ImGuiWindowFlags_NoScrollbar
                              | ImGuiWindowFlags_NoSavedSettings;
};

} // ui

#endif //IOFPRB_TESTMENU_H
