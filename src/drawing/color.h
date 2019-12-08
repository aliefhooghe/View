#ifndef VIEW_COLOR_H_
#define VIEW_COLOR_H_

#include <algorithm>
#include <cstdint>

namespace View {

    //  4 bytes : From most to least significant bit : RGBA
    using color = uint32_t;

    constexpr color make_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255u)
    {
        return (red << 24) | (green << 16) | (blue << 8) | alpha;
    }

    constexpr uint8_t get_red(color c)      { return (c >> 24) & 0xFF; }
    constexpr uint8_t get_green(color c)    { return (c >> 16) & 0xFF; }
    constexpr uint8_t get_blue(color c)     { return (c >> 8) & 0xFF; }
    constexpr uint8_t get_alpha(color c)    { return c & 0xFF; }

    constexpr double get_red_d(color c)     { return static_cast<double>(get_red(c)) / 255.; }
    constexpr double get_green_d(color c)   { return static_cast<double>(get_green(c)) / 255.; }
    constexpr double get_blue_d(color c)    { return static_cast<double>(get_blue(c)) / 255.; }
    constexpr double get_alpha_d(color c)   { return static_cast<double>(get_alpha(c)) / 255.; }
}

#endif