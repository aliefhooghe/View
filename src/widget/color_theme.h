#ifndef VIEW_COLOR_THEME_H
#define VIEW_COLOR_THEME_H

#include "drawing/color.h"

namespace View {

    /**
     * \brief
     */
    struct color_theme {
        color primary = 0x1289A7FF;
        color primary_light = 0x12CBC4FF;
        color primary_dark = 0x0652DDFF;
        color on_primary = 0xFFFFFFFF;

        color secondary = 0xfa983aFF;
        color secondary_light = 0xf6b93bFF;
        color secondary_dark = 0xe58e26FF;
        color on_secondary = 0xFFFFFFFF;

        color surface = 0x404040FF;
        color surface_light = 0x505050FF;
        color surface_dark = 0x252525ff;
        color on_surface = 0xFFFFFFFF;

        color background = 0x101010FF;
        color on_background = 0xFFFFFFFF;

        color error = 0xFF0000FF;
        color on_error = 0x000000FF;
    };

    constexpr auto default_color_theme = color_theme{};
}

#endif //VIEW_COLOR_THEME_H