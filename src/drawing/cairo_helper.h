#ifndef VIEW_CAIRO_HELPER_H_
#define VIEW_CAIRO_HELPER_H_

#include "../cairo_definition.h"
#include "color.h"

namespace View {

    /**
     *  \brief Set cairo source with given View color
     *  \param cr cairo context
     *  \param c color to use source
     */
    void set_source(cairo_t *cr, color c);

    /**
     *  \brief Draw a rounded rectangle
     */
    void rounded_rectangle(cairo_t * cr, float x, float y, float width, float height, float radius);

    /**
     *  \brief Circle
     */
    void circle(cairo_t *cr, float x, float y, float radius);

}

#endif