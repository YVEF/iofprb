#ifndef IOFPRB_WORKING_SURFACE_H
#define IOFPRB_WORKING_SURFACE_H
#include "base_element.h"
#include "../../utils/typedefs.h"
#include "../../providers/driveprv.h"

namespace ui {

class working_surface : public base_element
{
    ImGuiWindowFlags flags_ = ImGuiWindowFlags_NoTitleBar
                              | ImGuiWindowFlags_NoResize
                              | ImGuiWindowFlags_NoMove
                              | ImGuiWindowFlags_NoScrollbar
                              | ImGuiWindowFlags_NoSavedSettings;

    std::vector<providers::disk_draw_info> diskinfos_;

public:
    explicit working_surface(std::vector<providers::disk_draw_info> diskinfos) noexcept : diskinfos_(std::move(diskinfos)) {}
    void render(render_context& ctx) noexcept override;

};

} // ui

#endif //IOFPRB_WORKING_SURFACE_H
