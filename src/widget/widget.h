#ifndef VIEW_WIDGET_H_
#define VIEW_WIDGET_H_

#include "rectangle.h"
#include "cairo_definition.h"
#include "event.h"
#include "display/common/cursor.h"
#include "size_constraint.h"
#include "color_theme.h"

namespace View {

    class display_controler;

    class widget {
        //  Note : widget is not a rectangle because it does not know its position
        friend class display_controler;
    public:
        widget(float width, float height) noexcept;
        widget(float width, float height, size_constraint width_constraint, size_constraint height_constraint) noexcept;
        widget(widget&&) = default;
        widget(widget&) = delete;
        virtual ~widget();

        float width() const noexcept    { return _width; }
        float height() const noexcept   { return _height; }

        /** Size constraints **/
        auto& width_constraint() const noexcept  { return _width_constraint; }
        auto& height_constraint() const noexcept { return _height_constraint; }
        void freeze_size();

        virtual bool resize(float width, float height);
        bool resize_width(float w) { return resize(w, height()); }
        bool resize_height(float h) { return resize(width(), h); }

        virtual bool contains(float x, float y);

        //  Events
        virtual bool on_key_up(const keycode key)       { return false; }
        virtual bool on_key_down(const keycode key)     { return false; }

        virtual bool on_mouse_enter()                   { return false ; }
		virtual bool on_mouse_exit()                    { return false ; }
		virtual bool on_mouse_move(float x, float y)    { return false ; }
		virtual bool on_mouse_wheel(float distance)     { return false ; }

		virtual bool on_mouse_button_down(const mouse_button button, float x, float y)  { return false; }
		virtual bool on_mouse_button_up(const mouse_button button, float x, float y)    { return false; }
		virtual bool on_mouse_dbl_click(float x, float y)                               { return false; }

		virtual bool on_mouse_drag(const mouse_button button, float x, float y, float dx, float dy)     { return false; }
		virtual bool on_mouse_drag_start(const mouse_button button, float x, float y)                   { return false; }
		virtual bool on_mouse_drag_end(const mouse_button button, float x, float y)                     { return false; }
        virtual bool on_mouse_drag_cancel()                                                             { return false; }

        virtual void draw(cairo_t*) {}
        virtual void draw_rect(cairo_t* cr, const rectangle<>&) { draw(cr); }

        //  Color theme handling
        virtual void apply_color_theme(const color_theme& theme) {}

    protected:
        /** display control helpers **/
        void invalidate();
        void invalidate_rect(const rectangle<>&);
        void set_cursor(cursor cursor);

        void set_size_constraints(size_constraint width, size_constraint height) noexcept
        {
            _width_constraint = width;
            _height_constraint = height;
        }

    private:
        display_controler *_display_ctl{nullptr};
        float _width;
        float _height;
        size_constraint _width_constraint;
        size_constraint _height_constraint;
    };

}

#endif