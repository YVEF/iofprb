#ifndef IOFPRB_NEXTBACK_CANVAS_H
#define IOFPRB_NEXTBACK_CANVAS_H

#include "base_element.h"

#define NB_CANV_REGULAR_OVERRIDE        \
int next() const noexcept override;     \

namespace ui {

class nextback_canvas : public base_element
{
public:
    virtual int next() const noexcept = 0;
};

} // ui

#endif //IOFPRB_NEXTBACK_CANVAS_H
