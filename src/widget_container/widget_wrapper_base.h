#ifndef VIEW_WIDGET_WRAPPER_BASE_H_
#define VIEW_WIDGET_WRAPPER_BASE_H_

#include "widget_container.h"

namespace View {

    template <typename Derived, typename TWidgetHolder = widget_holder<>>
    class widget_wrapper_base : public widget_container<widget_wrapper_base<Derived, TWidgetHolder>> {
        friend class widget_container<widget_wrapper_base>;
    public:
        widget_wrapper_base(std::unique_ptr<widget>&& root);
        widget_wrapper_base(std::unique_ptr<widget>&& root, float x, float y, float width, float height);
        widget_wrapper_base(
            std::unique_ptr<widget>&& root,
            float x, float y,
            float width, float height,
            size_constraint width_constrain, size_constraint height_constrain);
        ~widget_wrapper_base() override = default;

        void draw(NVGcontext *vg) override;
        void draw_rect(NVGcontext* vg, const rectangle<>& area) override;

    protected:
        template <typename TFunction>
        void foreach_holder(TFunction func) { func(_root); }
        auto widget_at(float x, float y)
        {
            return &_root;
        }

        TWidgetHolder _root;
    };

    template <typename Derived, typename TWidgetHolder>
    widget_wrapper_base<Derived, TWidgetHolder>::widget_wrapper_base(std::unique_ptr<widget>&& root)
    : widget_container<widget_wrapper_base<Derived, TWidgetHolder>> {root->width(), root->height(), root->width_constraint(), root->height_constraint()},
        _root{static_cast<Derived&>(*this), 0, 0, std::move(root)}
    {
    }

    template <typename Derived, typename TWidgetHolder>
    widget_wrapper_base<Derived, TWidgetHolder>::widget_wrapper_base(std::unique_ptr<widget>&& root, float x, float y, float width, float height)
    : widget_container<widget_wrapper_base<Derived, TWidgetHolder>>{width, height},
        _root{static_cast<Derived&>(*this), x, y, std::move(root)}
    {
    }

    template <typename Derived, typename TWidgetHolder>
    widget_wrapper_base<Derived, TWidgetHolder>::widget_wrapper_base(
        std::unique_ptr<widget>&& root,
        float x, float y,
        float width, float height,
        size_constraint width_constrain, size_constraint height_constrain)
    :   widget_container<widget_wrapper_base<Derived, TWidgetHolder>>{width, height, width_constrain, height_constrain},
        _root{static_cast<Derived&>(*this), x, y, std::move(root)}
    {
    }

    template <typename Derived, typename TWidgetHolder>
    void widget_wrapper_base<Derived, TWidgetHolder>::draw(NVGcontext *vg)
    {
        widget_container<widget_wrapper_base<Derived, TWidgetHolder>>::draw_widgets(vg);
    }

    template <typename Derived, typename TWidgetHolder>
    void widget_wrapper_base<Derived, TWidgetHolder>::draw_rect(NVGcontext* cr, const rectangle<>& area)
    {
        widget_container<widget_wrapper_base<Derived, TWidgetHolder>>::draw_widgets(cr, area);
    }
}

#endif