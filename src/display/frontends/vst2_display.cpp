
#include "vst2_display.h"

namespace View
{
    vst2_display::vst2_display(std::unique_ptr<view_backend>&& backend)
    : _backend{std::move(backend)}
    {}

    void vst2_display::open(void *parent_handle)
    {
        _backend->create_window("VIEW VST2", parent_handle);
    }

    void vst2_display::close()
    {
        _backend->close_window();
    }

    unsigned int vst2_display::px_width()
    {
        return _backend->px_width();
    }

    unsigned int vst2_display::px_height()
    {
        return _backend->px_height();
    }

    void vst2_display::text_input(int32_t index, intptr_t value, int32_t opt)
    {
        const char text[2] =
        {
            _convert_char(index, value, opt),
            '\0'
        };
        _backend->vst2_text_input(text);
    }

    char vst2_display::_convert_char(int32_t index, intptr_t value, int32_t opt)
    {
        constexpr auto backspace = 8;
        constexpr auto enter = 13;

        if (std::isalpha(index)) {
            const auto result = (opt == 1 ? std::toupper(index) : index);
            return static_cast<char>(result);
        }
        else if (std::isdigit(index)) {
            const char c = static_cast<char>(index);
            if (opt == 1) {
                return c;
            }
            else {
                switch (c) {
                case '0': return 'à';
                case '1': return '&';
                case '2': return 'é';
                case '3': return '"';
                case '4': return '\'';
                case '5': return '(';
                case '6': return '-';
                case '7': return 'è';
                case '8': return '_';
                case '9': return 'ç';
                default: return 0;
                }
            }

        }
        else if (index == backspace || index == enter) {
            return static_cast<char>(index);
        }
        else {
            return 0;
        }
    }

} // namespace View
