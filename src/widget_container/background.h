#ifndef VIEW_BACKGROUND_H_
#define VIEW_BACKGROUND_H_

#include "widget_container.h"
#include "drawing/color.h"

namespace View {

    class background : public widget_container<background> {
        friend class widget_container<background>;
        using widget_container<background>::widget_holder;
    public:
        background(std::unique_ptr<widget>&& root, color background_color = 0x1e272eff);
        ~background() override = default;

        bool resize(float width, float height) override;
        void draw(cairo_t *cr) override;

        void apply_color_theme(const color_theme &theme) override;
    protected:
        template <typename TFunction>
        void foreach_holder(TFunction func) { func(_root); }
        widget_holder *widget_at(float x, float y)
        {
            if (_root.get()->contains(x, y))
                return &_root;
            else
                return nullptr;
        }

    private:
        widget_holder _root;
        color _background_color;
    };
}

#endif