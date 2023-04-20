#ifndef IOFPRB_BASE_ELEMENT_H
#define IOFPRB_BASE_ELEMENT_H

#include <imgui.h>
#include <vector>
#include <memory>
#include "../render_context.h"

namespace ui {

class base_element
{
protected:
    std::vector<std::shared_ptr<base_element>> child_elements_;

public:
    virtual void render(render_context& ctx) noexcept = 0;
    virtual ~base_element() noexcept = default;
};

} // ui

#endif //IOFPRB_BASE_ELEMENT_H
