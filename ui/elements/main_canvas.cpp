#include "main_canvas.h"

namespace ui {

void main_canvas::render(ui::render_context& ctx) noexcept
{
    int next = c_canvas_->next();
    if(next)
    {
        c_step_ += next;
        assert(c_step_ < steps_.size());
        c_canvas_ = steps_[c_step_](config_, driveprv_);
        c_canvas_->adjust();
    }

    c_canvas_->render(ctx);
}

main_canvas::main_canvas(config_state& config, const ambient::driveprv& driveprv) noexcept
: config_(config), c_step_(0), driveprv_(driveprv)
{
    c_canvas_ = steps_[c_step_](config_, driveprv_);
}

void main_canvas::adjust() noexcept
{
    c_canvas_->adjust();
}

} // ui