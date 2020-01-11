#include "text_helper.h"

namespace View {

    void draw_centered_text(
        cairo_t *cr,
        float x, float y, float width, float height, float size,
        const char *txt,
        color c,
        bool bold)
    {
        PangoLayout *layout;
        PangoFontDescription *font_description;

        font_description = pango_font_description_new ();
        pango_font_description_set_family_static(font_description, "sans");
        pango_font_description_set_weight(font_description, bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
        pango_font_description_set_absolute_size (font_description, size * static_cast<float>(PANGO_SCALE));

        layout = pango_cairo_create_layout (cr);
        pango_layout_set_font_description (layout, font_description);
        pango_layout_set_text (layout, txt, -1);

        PangoRectangle extent;
        pango_layout_get_pixel_extents(layout, &extent, nullptr);

        set_source(cr, c);
        cairo_move_to (
            cr,
            x + (width - static_cast<float>(extent.width)) / 2.f,
            y + (height - static_cast<float>(extent.height)) / 2.f);
        pango_cairo_show_layout (cr, layout);

        g_object_unref (layout);
        pango_font_description_free (font_description);
    }


}