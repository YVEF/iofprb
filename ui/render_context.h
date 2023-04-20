#ifndef IOFPRB_RENDER_CONTEXT_H
#define IOFPRB_RENDER_CONTEXT_H

#include <cstdlib>
#include "style.h"
#include <imgui.h>

namespace ui {

class render_context
{
    ImVec2 offset_;

public:
    explicit render_context(const style& st) noexcept
    : styles(st) {}

    const style& styles;
    void add_offset(ImVec2 off) noexcept
    {
        offset_.x += off.x;
        offset_.y += off.y;
    }

    ImVec2 get_offset() const noexcept { return offset_; }
};

}


#endif //IOFPRB_RENDER_CONTEXT_H
