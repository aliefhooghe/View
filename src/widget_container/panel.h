#ifndef VIEW_FREE_PANEL_H_
#define VIEW_FREE_PANEL_H_

#include <vector>
#include <algorithm>
#include "widget_container.h"

namespace View {

    template <typename TChildren = widget>
    class panel_implementation : public widget_container<panel_implementation<TChildren>, TChildren> {
        friend class widget_container<panel_implementation<TChildren>, TChildren>;
        using implementation = widget_container<panel_implementation<TChildren>, TChildren>;
    public:
        panel_implementation(float width, float height)
        :   implementation{width, height}
        {}

        panel_implementation(float width, float height, size_constraint width_constraint, size_constraint height_constraint)
        :   implementation{width, height, width_constraint, height_constraint}
        {}

        ~panel_implementation() override = default;

        // drawing funcs
        void draw(NVGcontext *vg) override
        {
            implementation::draw_widgets(vg);
        }

        void draw_rect(NVGcontext *vg, const rectangle<>&rect) override
        {
            implementation::draw_widgets(vg, rect);
        }

    protected:
        widget_holder<TChildren>& insert_widget(float x, float y, std::unique_ptr<TChildren>&& w)
        {
            auto& ret = _childrens.emplace_back(*this, x, y, std::move(w));
            implementation::invalidate();
            return ret;
        }

        void remove_widget(TChildren *children)
        {
            /**
             *  Removing an element from _childrens will invalidate any holder pointer or reference
             **/
            auto focused_widget = implementation::focused_widget();
            if (focused_widget != nullptr && focused_widget->get() == children) {
                children->on_mouse_exit();
                implementation::reset_focused_widget();
            }

            _childrens.erase(std::remove_if(
                _childrens.begin(), _childrens.end(),
                [children](const auto& holder)
                {
                    return (holder.get() == children);
                }),
                _childrens.end());

            implementation::invalidate();
        }

        widget_holder<TChildren> *widget_at(float x, float y)
        {
            //  Last widget are in foreground
            // TODO : Optimize (kd-tree ?)
            for (auto it = _childrens.rbegin(); it != _childrens.rend(); ++it) {
                const auto x_rel = x - it->pos_x();
                const auto y_rel = y - it->pos_y();
                if (it->get()->contains(x_rel, y_rel))
                    return &(*it);
            }
            //  No widget at this position
            return nullptr;
        }

        template <typename TFunction>
        void foreach_holder(TFunction func)
        {
            std::for_each(_childrens.begin(), _childrens.end(), func);
        }

        std::vector<widget_holder<TChildren>> _childrens{};
    };


    template <typename TChildren = widget>
    class panel : public panel_implementation<TChildren> {

        public:
            panel(float width, float height)
            :   panel_implementation<TChildren>(width, height)
            {}

            panel(float width, float height, size_constraint width_constraint, size_constraint height_constraint)
            : panel_implementation<TChildren>(width, height, width_constraint, height_constraint)
            {}

            using panel_implementation<TChildren>::insert_widget;
            using panel_implementation<TChildren>::remove_widget;
    };

}

#endif