#ifndef VIEW_HEADER_H_
#define VIEW_HEADER_H_

#include "widget_container.h"
#include "drawing/color.h"

namespace View {

    class header : public widget_container<header> {
        friend class widget_container<header>;
        using widget_holder = widget_container<header>::widget_holder;
    public:
        header(
            std::unique_ptr<widget>&& root,
            float header_size = 4.f, float border_size = 1.f,
            color background_color = 0x2A2A2EFFu, color header_color = 0x23D18BFF);

        ~header() override = default;

        bool resize(float width, float height) override;
        void draw(cairo_t *cr) override;
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

        widget_holder _root;
    private:
        float _header_size;
        float _border_size;
        color _background_color;
        color _header_color;
    };

}

#endif
