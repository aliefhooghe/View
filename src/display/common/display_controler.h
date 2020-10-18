#ifndef VIEW_DISPLAY_CONTROLER_H_
#define VIEW_DISPLAY_CONTROLER_H_

#include <exception>

#include "widget/widget.h"
#include "cursor.h"

#include <iostream>

namespace View {

    /**
     *  \class display_controler
     *  \brief Allow one widget to have some control of underlying display
     */
    class display_controler {
        friend class widget;
    public:
        display_controler() noexcept
        : _widget{nullptr}
        {}

        display_controler(widget& w) noexcept
        : _widget{&w}
        {
            _widget->_display_ctl = this;
        }

        display_controler(const display_controler& other) = delete;
        display_controler(display_controler&& other) noexcept
        {
            _widget = other._widget;
            if (_widget)
                _widget->_display_ctl = this;
            other._widget = nullptr;
        }

        void set_widget(widget& w)
        {
            _detach();
            _widget = &w;
            _widget->_display_ctl = this;
        }

        virtual ~display_controler()
        {
            _detach();
        }

        /**
         *  \brief Inform the display that a widget area need to be redrawn
         *  \param rect the are which need to be redrawn (widget coordinate)
         **/
        virtual void invalidate_rect(const rectangle<>& rect) =0;

        /**
         *  \brief Inform the display that the widget need to be redrawn
         **/
        virtual void invalidate_widget()
        {
            if (_widget != nullptr)
                invalidate_rect(
                    make_rectangle(
                        0u, _widget->height(),
                        0u, _widget->width()));
        }

        /**
         *  \brief Change the current cursor
         *
         **/
        virtual void set_cursor(cursor /* = default */) =0;

        /**
         *  \brief Allow the widget top know its position
         **/
        virtual float widget_pos_x() { return 0.f; };
        virtual float widget_pos_y() { return 0.f; };
    private:
        void _detach()
        {
            if (_widget && _widget->_display_ctl == this) {
                _widget->_display_ctl = nullptr;
                _widget = nullptr;
            }
        }
        widget *_widget{nullptr};
    };

}

#endif