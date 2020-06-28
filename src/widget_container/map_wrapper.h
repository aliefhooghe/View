#ifndef VIEW_MAP_PANEL_H_
#define VIEW_MAP_PANEL_H_

#include "widget_wrapper_base.h"

namespace View {

    class map_wrapper;
    class map_wrapper_widget_holder : public widget_holder<> {
    public:
        map_wrapper_widget_holder(
            map_wrapper& parent, float x, float y,
            std::unique_ptr<widget>&& children);

    protected:
        void invalidate_rect(const rectangle<>& rect) override;
        void invalidate_widget() override;

        map_wrapper *_parent;
    };

    class map_wrapper : public widget_wrapper_base<map_wrapper, map_wrapper_widget_holder> {
        friend class map_wrapper_widget_holder;

    public:
        map_wrapper(
            std::unique_ptr<widget>&& root,
            float width, float height);

        map_wrapper(
            std::unique_ptr<widget>&& root,
            float width, float height,
            size_constraint width_constrain, size_constraint height_constrain);

        void recenter() noexcept;

        //  Events with coordinate
		bool on_mouse_move(float x, float y) override;
		bool on_mouse_button_down(const mouse_button button, float x, float y) override;
		bool on_mouse_button_up(const mouse_button button, float x, float y) override;
		bool on_mouse_dbl_click(float x, float y) override;
		bool on_mouse_drag(const mouse_button button, float x, float y, float dx, float dy) override;
		bool on_mouse_drag_start(const mouse_button button, float x, float y) override;
		bool on_mouse_drag_end(const mouse_button button, float x, float y) override;

        void draw(cairo_t *cr) override;
        void draw_rect(cairo_t* cr, const rectangle<>& area) override { draw(cr); }
        /** @todo draw_rect ! **/

    protected:
        void _translate_origin(float dx, float dy) noexcept;
        void _set_origin(float x, float y) noexcept;

        float _x_to_content(float x) const noexcept { return x + _origin_x; }
        float _y_to_content(float y) const noexcept { return y + _origin_y; }

    private:
        float _origin_x{0.f};
        float _origin_y{0.f};
        float _scale{1.0f};
    };

}

#endif