
#include "layout_separator.h"

namespace View {

    layout_separator::layout_separator(float width, float height, orientation orient)
    :    control{width, height, make_size_cursor(orthogonal(orient))},
        _orientation{orient}
    {
    }

    bool layout_separator::on_mouse_drag(mouse_button b, float x, float y, float dx, float dy)
    {
        if (b == mouse_button::left) {
            if (_orientation == orientation::horizontal) {
                if (dy != 0.f)
                    _callback(dy);
            }
            else { // vertical
                if (dx != 0.f)
                    _callback(dx);
            }

            return true;
        }
        else {
            return false;
        }
    }

}