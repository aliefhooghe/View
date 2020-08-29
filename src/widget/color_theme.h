#ifndef VIEW_COLOR_THEME_H
#define VIEW_COLOR_THEME_H

#include <nanovg.h>
#include <cstdint>

namespace View {

    /**
     * \brief Convert an integer into a color
     * \note we do not use nanovg functions as we want to
     * enable compile time comuitations
     */
    constexpr auto color_from_integer(const uint32_t n)
    {
        NVGcolor color;
        color.r = ((n & 0xFF000000) >> 24) / 255.f;
        color.g = ((n & 0x00FF0000) >> 16) / 255.f; 
        color.b = ((n & 0x0000FF00) >> 8) / 255.f;
        color.a = (n & 0x000000FF) / 255.f;
        return color;
    }

    /**
     * \brief
     */
    struct color_theme {
        NVGcolor primary = color_from_integer(0x1289A7FF);
        NVGcolor primary_light = color_from_integer(0x12CBC4FF);
        NVGcolor primary_dark = color_from_integer(0x0652DDFF);
        NVGcolor on_primary = color_from_integer(0xFFFFFFFF);

        NVGcolor secondary = color_from_integer(0xfa983aFF);
        NVGcolor secondary_light = color_from_integer(0xf6b93bFF);
        NVGcolor secondary_dark = color_from_integer(0xe58e26FF);
        NVGcolor on_secondary = color_from_integer(0xFFFFFFFF);

        NVGcolor surface = color_from_integer(0x404040FF);
        NVGcolor surface_light = color_from_integer(0x505050FF);
        NVGcolor surface_dark = color_from_integer(0x252525ff);
        NVGcolor on_surface = color_from_integer(0xFFFFFFFF);

        NVGcolor background = color_from_integer(0x101010FF);
        NVGcolor on_background = color_from_integer(0xFFFFFFFF);

        NVGcolor error = color_from_integer(0xFF0000FF);
        NVGcolor on_error = color_from_integer(0x000000FF);
    };

    constexpr auto default_color_theme = color_theme{};
}

#endif //VIEW_COLOR_THEME_H