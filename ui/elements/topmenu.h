#ifndef IOFPRB_TOPMENU_H
#define IOFPRB_TOPMENU_H
#include "base_element.h"

namespace ui {
class topmenu : public base_element
{
public:
    BASE_EL_REGULAR_OVERRIDE

private:
    ImGuiWindowFlags flags_ = ImGuiWindowFlags_NoResize
                              | ImGuiWindowFlags_NoMove
                              | ImGuiWindowFlags_NoScrollbar
                              | ImGuiWindowFlags_NoSavedSettings;
};

}


#endif //IOFPRB_TOPMENU_H
