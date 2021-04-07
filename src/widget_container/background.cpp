
#include "background.h"
#include "drawing/text_helper.h"

namespace View {

    background::background(std::unique_ptr<widget>&& root)
    :   widget_wrapper_base<background>{std::move(root)}
    {
        apply_color_theme(default_color_theme);
    }

    bool background::resize(float width, float height)
    {
        if (_root->resize(width, height))
        {
            widget_wrapper_base<background>::resize(width, height);
            return true;
        }
        else {
            return false;
        }
    }

    void background::draw(NVGcontext *vg)
    {
        //  Draw background
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, width(), height());
        nvgFillColor(vg, _background_color);
        nvgFill(vg);

        //  Draw widgets
        widget_wrapper_base<background>::draw(vg);
    }

    void background::draw_rect(NVGcontext *vg, const rectangle<>& area)
    {
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, width(), height());
        nvgFillColor(vg, _background_color);
        nvgFill(vg);

        //  Draw widgets
        widget_wrapper_base<background>::draw_rect(vg, area);
    }

    void background::apply_color_theme(const View::color_theme &theme)
    {
        //  apply on childrens
        widget_wrapper_base<background>::apply_color_theme(theme);

        //  set background color
        _background_color = theme.background;
    }
}