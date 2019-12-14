#ifndef VIEW_WIDGET_H_
#define VIEW_WIDGET_H_

#include "rectangle.h"
#include "cairo_definition.h"
#include "event.h"
#include "display/common/cursor.h"

namespace View {

    class display_controler;

    class widget {
        //  Note : widget is not a rectangle because it does not know its position
        friend class display_controler;
    public:
        widget(float width, float height) noexcept;
        widget(widget&&) = default;
        widget(widget&) = delete;
        virtual ~widget() = default;

        virtual bool resize(float width, float height);
        float width() const noexcept    { return _width; }
        float height() const noexcept   { return _height; }
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
        virtual void draw_rect(cairo_t*, const rectangle<>&) {};

    protected:
        /** display control helpers **/
        void invalidate();
        void invalidate_rect(const rectangle<>&);
        void set_cursor(cursor cursor);

    private:
        display_controler *_display_ctl{nullptr};
        float _width;
        float _height;
    };

}

#endif