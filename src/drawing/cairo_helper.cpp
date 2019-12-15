
#include "cairo_helper.h"
#include <cmath>

namespace View {

    void set_source(cairo_t *cr, color c)
    {
        cairo_set_source_rgba(
            cr, get_red_d(c), get_green_d(c), get_blue_d(c), get_alpha_d(c));
    }

    void rounded_rectangle(cairo_t * cr, float x, float y, float width, float height, float radius)
	{
		cairo_new_sub_path(cr);
		cairo_arc(cr, x + width - radius, y + radius, radius, -M_PI_2, 0.0);
		cairo_arc(cr, x + width - radius, y + height - radius, radius, 0.0, M_PI_2);
		cairo_arc(cr, x + radius, y + height - radius, radius, M_PI_2, M_PI);
		cairo_arc(cr, x + radius, y + radius, radius, M_PI, M_PI + M_PI_2);
		cairo_close_path(cr);
	}

    void circle(cairo_t *cr, float x, float y, float radius)
    {
        cairo_arc(cr, x, y, radius, 0.0, 2.0 * M_PI);
    }

}