#ifndef VIEW_WIDGET_WRAPPER_BASE_H_
#define VIEW_WIDGET_WRAPPER_BASE_H_

#include "widget_container.h"

namespace View {

    class widget_wrapper_base : public widget_container<widget_wrapper_base> {
        friend class widget_container<widget_wrapper_base>;
        using typename widget_container<widget_wrapper_base>::widget_holder;
    public:
        widget_wrapper_base(std::unique_ptr<widget>&& root);
        widget_wrapper_base(std::unique_ptr<widget>&& root, float width, float height);
        widget_wrapper_base(
            std::unique_ptr<widget>&& root,
            float width, float height,
            size_constraint width_constrain, size_constraint height_constrain);
        ~widget_wrapper_base() override = default;

        void draw(cairo_t *cr) override;
        void draw_rect(cairo_t* cr, const rectangle<>& area) override;

    protected:
        template <typename TFunction>
        void foreach_holder(TFunction func) { func(_root); }
        widget_holder *widget_at(float x, float y)
        {
            return &_root;
        }

        widget_holder _root;
    };

}

#endif