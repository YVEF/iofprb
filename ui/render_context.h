#ifndef IOFPRB_RENDER_CONTEXT_H
#define IOFPRB_RENDER_CONTEXT_H

#include <cstdlib>
#include "st_color.h"
#include <imgui.h>
#include "../ambient/config_state.h"

namespace ui {

class render_context
{
public:
    explicit render_context(const st_color& st) noexcept
    : colors(st) {}

    const st_color& colors;
    const float standard_space = 15.f;

    void add_offset(const ImVec2& off) noexcept
    {
        offset_.x += off.x;
        offset_.y += off.y;
    }

    [[nodiscard]] ImVec2 get_offset() const noexcept { return offset_; }

private:
    ImVec2 offset_;
};

}


#endif //IOFPRB_RENDER_CONTEXT_H
