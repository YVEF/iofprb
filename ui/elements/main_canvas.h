#ifndef IOFPRB_MAIN_CANVAS_H
#define IOFPRB_MAIN_CANVAS_H
#include "nextback_canvas.h"
#include "start_menu.h"
#include "bench_chart.h"
#include <memory>
#include <array>
#include "testmenu.h"

namespace ui {

class main_canvas : public base_element
{
public:
    BASE_EL_REGULAR_OVERRIDE
    void adjust() noexcept override;
    explicit main_canvas(config_state&, const providers::driveprv&) noexcept;

private:
    config_state& config_;
    std::unique_ptr<nextback_canvas> c_canvas_;
    uint c_step_;
    const providers::driveprv& driveprv_;

    std::array<std::unique_ptr<nextback_canvas> (*)(config_state&, const providers::driveprv&), 2> steps_ = {
            &get_next_canvas_<start_menu>,
            &get_next_canvas_<bench_chart>
    };

    template<typename T>
    static std::unique_ptr<nextback_canvas> get_next_canvas_(config_state& config, const providers::driveprv& drv)
    {
        auto canvas = std::make_unique<T>(config, drv);

        return canvas;
    }
};

} // ui

#endif //IOFPRB_MAIN_CANVAS_H
