#ifndef VIEW_FREE_PANEL_H_
#define VIEW_FREE_PANEL_H_

#include <vector>
#include <algorithm>
#include "widget_container.h"

namespace View {

    template <typename TChildren>
    class panel : public widget_container<panel<TChildren>, TChildren> {
        friend class widget_container<panel<TChildren>, TChildren>;
        using widget_holder = typename widget_container<panel<TChildren>, TChildren>::widget_holder;
    public:
        panel(float width, float height)
        :   widget_container<panel<TChildren>, TChildren>{width, height}
        {}

        panel(float width, float height, size_constraint width_constraint, size_constraint height_constraint)
        :   widget_container<panel<TChildren>, TChildren>{width, height, width_constraint, height_constraint}
        {}

        ~panel() override = default;

        // drawing funcs
        void draw(cairo_t* cr) override
        {
            draw_background(cr);
            widget_container<panel<TChildren>, TChildren>::draw_widgets(cr);
            draw_foreground(cr);
        }

        void draw_rect(cairo_t* cr, const rectangle<>&rect) override
        {
            /**
             * \todo : this does not work
             **/
            draw_background(cr);
            widget_container<panel<TChildren>, TChildren>::draw_widgets(cr, rect);
            draw_foreground(cr);
        }

    protected:
        virtual void draw_background(cairo_t *cr) {}
        virtual void draw_foreground(cairo_t *cr) {}

        void insert_widget(float x, float y, std::unique_ptr<TChildren>&& w)
        {
            _childrens.emplace_back(*this, x, y, std::move(w));
        }

        void remove_widget(TChildren *children)
        {
            _childrens.erase(std::remove_if(
                _childrens.begin(), _childrens.end(),
                [children](const auto& holder)
                {
                    return holder.is(children);
                }),
                _childrens.end());

            /** \todo : find a solution **/
            // if (_focused_widget == children)
            //     _focused_widget = nullptr;
        }

        widget_holder *widget_at(float x, float y)
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

        std::vector<widget_holder> _childrens{};
    };

}

#endif