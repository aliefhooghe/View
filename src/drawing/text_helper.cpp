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

    void draw_centered_text(
        cairo_t *cr,
        float x, float y, float width, float height, float size,
        const char *txt,
        color c,
        bool bold)
    {
        PangoLayout *layout;
        PangoFontDescription *font_description;

        font_description = pango_font_description_new();

        pango_font_description_set_family_static(font_description, "Noto");
        pango_font_description_set_weight(font_description, bold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_LIGHT);
        pango_font_description_set_absolute_size (font_description, size * static_cast<float>(PANGO_SCALE));
        pango_font_description_set_stretch(font_description, PANGO_STRETCH_NORMAL);
        pango_font_description_set_style(font_description, PANGO_STYLE_NORMAL);

        PangoAttrList *attributes = pango_attr_list_new();
        pango_attr_list_insert(attributes, pango_attr_letter_spacing_new(0.1 * size * static_cast<float>(PANGO_SCALE)));

        layout = pango_cairo_create_layout (cr);
        pango_layout_set_font_description (layout, font_description);
        pango_layout_set_text (layout, txt, -1);
        pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
        pango_layout_set_width(layout, static_cast<int>(width) * PANGO_SCALE);
        pango_layout_set_height(layout, static_cast<int>(height) * PANGO_SCALE);
        pango_layout_set_attributes(layout, attributes);

        set_source(cr, c);
        cairo_move_to(cr, x, y);
        pango_cairo_show_layout (cr, layout);

        g_object_unref (layout);
        pango_attr_list_unref(attributes);
        pango_font_description_free (font_description);
    }

}