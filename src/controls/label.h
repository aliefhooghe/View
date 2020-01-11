#ifndef VIEW_LABEL_H_
#define VIEW_LABEL_H_

#include <string>

#include "widget/widget.h"
#include "drawing/text_helper.h"

namespace View {

    class label : public widget {
    public:
        template <typename Ttext>
        label(float width, float height, Ttext&& str, float font_size, color c = 0x000000FFu, bool bold = false)
        : widget{10, 10},
            _text{std::forward<Ttext>(str)},
            _bold{bold}, _color{c}, _font_size{font_size}
        {}
        ~label() override = default;

        template <typename Ttext>
        void set_text(Ttext text) noexcept { _text = text; invalidate(); }
        void set_bold(bool bold) noexcept { _bold = bold; invalidate(); }
        void set_color(color c) noexcept { _color = c; invalidate(); }
        void set_font_size(float size) noexcept { _font_size = size; invalidate(); }

        void draw(cairo_t *cr) override
        {
            draw_centered_text(cr, 0, 0, width(), height(), _font_size, _text.c_str(), _color, _bold);
        }

    private:
        std::string _text;
        bool _bold;
        color _color;
        float _font_size;
    };

}

#endif