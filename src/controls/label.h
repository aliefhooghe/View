#ifndef VIEW_LABEL_H_
#define VIEW_LABEL_H_

#include <string>

#include "widget/widget.h"
#include "drawing/text_helper.h"

namespace View {

    class label : public widget {
    public:
        template <typename Ttext>
        label(float width, float height, Ttext&& str, float font_size = 14.f, bool bold = false)
        : widget{width, height},
            _text{std::forward<Ttext>(str)},
            _bold{bold}, _font_size{font_size},
            _ha{horizontal_alignment::center},
            _va{vertical_alignment::center}
        {
            apply_color_theme(default_color_theme);
        }

        template <typename Ttext>
        label(Ttext&& str, float font_size = 14.f, bool bold = false)
        : widget{1., font_size},
            _text{std::forward<Ttext>(str)},
            _bold{bold}, _font_size{font_size},
            _ha{horizontal_alignment::left},
            _va{vertical_alignment::center}
        {
        }

        ~label() override = default;

        void apply_color_theme(const color_theme& theme) override;

        template <typename Ttext>
        void set_text(Ttext text) noexcept { _text = text; invalidate(); }

        void set_bold(bool bold) noexcept;
        void set_color(NVGcolor c) noexcept;
        void set_font_size(float size) noexcept;

        void draw(NVGcontext *vg) override;
    private:
        std::string _text;
        bool _bold;
        NVGcolor _color;
        float _font_size;
        const horizontal_alignment _ha;
        const vertical_alignment _va;
    };

}

#endif