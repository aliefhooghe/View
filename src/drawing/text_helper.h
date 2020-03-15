#ifndef VIEW8TEXT_HELPER_H_
#define VIEW8TEXT_HELPER_H_

#include <pango/pangocairo.h>
#include "cairo_helper.h"

namespace View {

    void draw_centered_text(
        cairo_t *cr,
        float x, float y, float width, float height, float font_size,
        const char *txt,
        color c = 0x000000FFu,
        bool bold = false);

    void list_fonts();
}

#endif
