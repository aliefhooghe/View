#ifndef VIEW_DISPLAY_CONTROLER_H_
#define VIEW_DISPLAY_CONTROLER_H_

#include <exception>
#include "widget.h"

namespace View {

    /**
     *  \class display_controler
     *  \brief Allow one widget to have some control of underlying display
     */
    class display_controler {
    public:
        display_controler(widget& w)
        : _widget{w}
        {
            if (_widget._display_ctl == nullptr)
                _widget._display_ctl = this;
            else
                throw std::runtime_error("Tried to construst a second display controler for a widget");
        }

        display_controler(const display_controler&) = delete;
        display_controler(display_controler&&) noexcept = default;

        virtual ~display_controler()
        {
            _widget._display_ctl = nullptr;
        }

        /**
         *  \brief Inform the display that the widget need to be redrawn
         **/
        virtual void invalidate_widget() =0;

        /**
         *  \brief Inform the display that a widget area need to be redrawn
         *  \param rect the are which need to be redrawn (widget coordinate)
         **/
        virtual void invalidate_rect(const rectangle<>& rect) =0;

    private:
        widget& _widget;
    };

}

#endif