#include "text_helper.h"

namespace View {

    void list_fonts()
    {
        PangoFontFamily ** families;
        int n_families;
        PangoFontMap * fontmap = pango_cairo_font_map_get_default();

        pango_font_map_list_families (fontmap, & families, & n_families);

        for (auto i = 0; i < n_families; i++) {
            PangoFontFamily * family = families[i];
            const char * family_name = pango_font_family_get_name (family);
            printf ("Family %d: %s\n", i, family_name);
        }
        g_free (families);
    }

    static PangoFontDescription *setup_font_description(float size,bool bold)
    {
        PangoFontDescription *font_description = pango_font_description_new();

        pango_font_description_set_family_static(font_description, "Noto");
        pango_font_description_set_weight(font_description, bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_LIGHT);
        pango_font_description_set_absolute_size (font_description, size * static_cast<float>(PANGO_SCALE));
        pango_font_description_set_stretch(font_description, PANGO_STRETCH_NORMAL);
        pango_font_description_set_style(font_description, PANGO_STYLE_NORMAL);

        return font_description;
    }

    static constexpr auto horizontal_align_t2_pango_align(const horizontal_alignment h)
    {
        switch (h)
        {
            case horizontal_alignment::left: return PANGO_ALIGN_LEFT;
            case horizontal_alignment::center: return PANGO_ALIGN_CENTER;
            case horizontal_alignment::right: return PANGO_ALIGN_RIGHT;
        }
    }

    static constexpr auto get_aligned_text_y(
        float rect_y, float rect_height,
        float text_height, vertical_alignment va)
    {
        switch (va)
        {
        case vertical_alignment::top:
            return rect_y;
            break;

        case vertical_alignment::center:
            return rect_y + (rect_height - text_height) / 2.f;
            break;

        case vertical_alignment::bottom:
            return rect_y + rect_height - text_height;
            break;
        }
    }

    void draw_text(
        cairo_t *cr,
        float x, float y, float width, float height, float font_size,
        const char *txt,
        bool bold,
        horizontal_alignment ha,
        vertical_alignment va)
    {
        PangoLayout *layout = pango_cairo_create_layout (cr);
        PangoAttrList *attributes = pango_attr_list_new();
        PangoFontDescription *font_description = setup_font_description(font_size, bold);

        pango_attr_list_insert(attributes, pango_attr_letter_spacing_new(0.1 * font_size * static_cast<float>(PANGO_SCALE)));

        pango_layout_set_font_description (layout, font_description);
        pango_layout_set_text (layout, txt, -1);
        pango_layout_set_alignment(layout, horizontal_align_t2_pango_align(ha));
        pango_layout_set_width(layout, static_cast<int>(width) * PANGO_SCALE);
        pango_layout_set_height(layout, static_cast<int>(height) * PANGO_SCALE);
        pango_layout_set_attributes(layout, attributes);

        //  Vertical alignment is not handled by pango
        PangoRectangle extent_rect;
        pango_layout_get_pixel_extents(layout, nullptr, &extent_rect);

        const float y_text =
            get_aligned_text_y(y, height, extent_rect.height, va);

        cairo_move_to(cr, x, y_text);
        pango_cairo_show_layout (cr, layout);

        g_object_unref(layout);
        pango_attr_list_unref(attributes);
        pango_font_description_free (font_description);
    }

}