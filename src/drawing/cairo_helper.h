#ifndef VIEW_CAIRO_HELPER_H_
#define VIEW_CAIRO_HELPER_H_

#include "../cairo_definition.h"
#include "color.h"

namespace View {

    inline void set_source(cairo_t *cr, color c)
    {
        cairo_set_source_rgba(
            cr, get_red_d(c), get_green_d(c), get_blue_d(c), get_alpha_d(c));
    }

}

#endif