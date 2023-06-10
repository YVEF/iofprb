#ifndef IOFPRB_BASE_ELEMENT_H
#define IOFPRB_BASE_ELEMENT_H

#include <imgui.h>
#include <vector>
#include <memory>
#include "../render_context.h"

#define BASE_EL_REGULAR_OVERRIDE                                                            \
void render(render_context& ctx) noexcept override;


namespace ui {

class base_element
{
public:
    virtual void render(render_context& ctx) noexcept = 0;
    virtual void adjust() noexcept {};
    virtual bool skip([[maybe_unused]] const render_context& ctx) const noexcept
    { return false; }
    virtual ~base_element() noexcept = default;


protected:
    std::vector<std::shared_ptr<base_element>> child_elements_;
};

} // ui

#endif //IOFPRB_BASE_ELEMENT_H
