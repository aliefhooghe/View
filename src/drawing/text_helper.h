#ifndef VIEW8TEXT_HELPER_H_
#define VIEW8TEXT_HELPER_H_

#include "cairo_helper.h"

namespace View {

    enum class horizontal_alignment {
        left, center, right
    };

    enum class vertical_alignment {
        top, center, bottom
    };

    void draw_text(
        cairo_t *cr,
        float x, float y, float width, float height, float font_size,
        const char *txt,
        bool bold = false,
        horizontal_alignment ha = horizontal_alignment::center,
        vertical_alignment va = vertical_alignment::center);

}

#endif
