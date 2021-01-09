#ifndef VIEW_WIDGET_H_
#define VIEW_WIDGET_H_

#include <string_view>

#include <nanovg.h>
#include "color_theme.h"
#include "display/common/cursor.h"
#include "event.h"
#include "rectangle.h"
#include "size_constraint.h"

namespace View {

    class display_controler;

    class widget {
        //  Note : widget is not a rectangle because it does not know its position
        friend class display_controler;
    public:
        widget(float width, float height) noexcept;
        widget(float width, float height, size_constraint width_constraint, size_constraint height_constraint);
        widget(widget&&) = default;
        widget(widget&) = delete;
        virtual ~widget();

        float width() const noexcept    { return _width; }
        float height() const noexcept   { return _height; }

        /** Size constraints **/
        auto& width_constraint() const noexcept  { return _width_constraint; }
        auto& height_constraint() const noexcept { return _height_constraint; }
        bool constraint_match_size(float w, float h) const noexcept { return constraint_match_width(w) && constraint_match_height(h); }
        bool constraint_match_width(float w) const noexcept { return _width_constraint.contains(w); }
        bool constraint_match_height(float h) const noexcept { return _height_constraint.contains(h); }
        void freeze_size();

        virtual bool resize(float width, float height);
        bool resize_width(float w) { return resize(w, height()); }
        bool resize_height(float h) { return resize(width(), h); }

        virtual bool contains(float x, float y);

        //  Events
        virtual bool on_key_up(const keycode key)       { return false; }
        virtual bool on_key_down(const keycode key)     { return false; }
        virtual bool on_text_input(std::string_view)    { return false; }

        virtual bool on_mouse_enter()                   { return false; }
		virtual bool on_mouse_exit()                    { return false; }
		virtual bool on_mouse_move(float x, float y)    { return false; }
		virtual bool on_mouse_wheel(float distance)     { return false; }

		virtual bool on_mouse_button_down(const mouse_button button, float x, float y)  { return false; }
		virtual bool on_mouse_button_up(const mouse_button button, float x, float y)    { return false; }
		virtual bool on_mouse_dbl_click(float x, float y)                               { return false; }

		virtual bool on_mouse_drag(const mouse_button button, float x, float y, float dx, float dy)     { return false; }
		virtual bool on_mouse_drag_start(const mouse_button button, float x, float y)                   { return false; }
		virtual bool on_mouse_drag_end(const mouse_button button, float x, float y)                     { return false; }
        virtual bool on_mouse_drag_cancel()                                                             { return false; }

        virtual void draw(NVGcontext*) {}
        virtual void draw_rect(NVGcontext* vg, const rectangle<>&) { draw(vg); }

        //  Color theme handling
        virtual void apply_color_theme(const color_theme& theme) {}

        //  Position in parent
        float pos_x() const;
        float pos_y() const;

        /** display control helpers **/
        void invalidate();
        void invalidate_rect(const rectangle<>&);
        void set_cursor(cursor cursor);

    protected:
        void set_size_constraints(size_constraint width, size_constraint height) noexcept
        {
            _width_constraint = width;
            _height_constraint = height;
        }

        display_controler *display_ctl() const noexcept { return _display_ctl; }

    private:
        display_controler *_display_ctl{nullptr};
        float _width;
        float _height;
        size_constraint _width_constraint;
        size_constraint _height_constraint;
    };

}

#endif