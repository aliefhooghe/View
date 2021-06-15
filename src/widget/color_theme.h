#ifndef VIEW_COLOR_THEME_H
#define VIEW_COLOR_THEME_H

#include <nanovg.h>
#include <cstdint>

#undef ERROR

namespace View {

    /**
     * \brief Convert an integer into a color
     * \note we do not use nanovg functions as we want to
     * enable compile time comuitations
     */
    constexpr auto color_from_integer(const uint32_t n)
    {
        NVGcolor color{};
        color.rgba[0] = ((n & 0xFF000000) >> 24) / 255.f;
        color.rgba[1] = ((n & 0x00FF0000) >> 16) / 255.f; 
        color.rgba[2] = ((n & 0x0000FF00) >> 8) / 255.f;
        color.rgba[3] = (n & 0x000000FF) / 255.f;
        return color;
    }

    struct color_theme {
        constexpr color_theme() = default;

        /**
         * \brief a tag used to identify a color in a given color theme
         */
        enum class color {
            PRIMARY, PRIMARY_LIGHT, PRIMARY_DARK, ON_PRIMARY,
            SECONDARY, SECONDARY_LIGHT, SECONDARY_DARK, ON_SECONDARY,
            SURFACE, SURFACE_LIGHT, SURFACE_DARK, ON_SURFACE,
            BACKGROUND, ON_BACKGROUND,
            ERROR, ON_ERROR
        };

        /**
         * \brief Retrieve a color in the them
         * \param tag color tag
         */
        constexpr auto get(const color tag) const noexcept
        {
            switch (tag)
            {
                default:
                case color::PRIMARY:            return primary;
                case color::PRIMARY_LIGHT:      return primary_light;
                case color::PRIMARY_DARK:       return primary_dark;
                case color::ON_PRIMARY:         return on_primary;
                case color::SECONDARY:          return secondary;
                case color::SECONDARY_LIGHT:    return secondary_light;
                case color::SECONDARY_DARK:     return secondary_dark;
                case color::ON_SECONDARY:       return on_secondary;
                case color::SURFACE:            return surface;
                case color::SURFACE_LIGHT:      return surface_light;
                case color::SURFACE_DARK:       return surface_dark;
                case color::ON_SURFACE:         return on_surface;
                case color::BACKGROUND:         return background;
                case color::ON_BACKGROUND:      return on_background;
                case color::ERROR:              return error;
                case color::ON_ERROR:           return on_error;
            }
        }

        /*
         *  Colors category inspired by material design 
         */

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

        NVGcolor error = color_from_integer(0xE63547FF);
        NVGcolor on_error = color_from_integer(0x000000FF);
    };

    constexpr auto default_color_theme = color_theme{};
}

#endif //VIEW_COLOR_THEME_H