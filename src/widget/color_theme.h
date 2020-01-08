#ifndef VIEW_COLOR_THEME_H
#define VIEW_COLOR_THEME_H

#include "drawing/color.h"

namespace View {

    /**
     * \brief
     */
    struct color_theme {
        color primary = 0x27AE60FF;
        color primary_light = 0x2ECC71FF;
        color primary_dark = 0x16A085FF;
        color on_primary = 0xFFFFFFFF;

        color secondary = 0xE74C3CFF;
        color seconday_light = 0xE67E22FF;
        color secondary_dark = 0xD35400FF;
        color on_secondary = 0xFFFFFFFF;

        color surface = 0x34495EFF;
        color on_surface = 0xFFFFFFFF;

        color background = 0x2C3E50FF;
        color on_background = 0xFFFFFFFF;
    };

    constexpr auto default_color_theme = color_theme{};
}

#endif //VIEW_COLOR_THEME_H