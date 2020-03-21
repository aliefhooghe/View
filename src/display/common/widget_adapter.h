#ifndef VIEW_WIDGET_ADAPTER_H_
#define VIEW_WIDGET_ADAPTER_H_

#include "widget/widget.h"
#include "display/common/display_controler.h"

namespace View {

    /**
     *  \class widget_adapter
     *  \brief widget_adapter is the brige beetween low level GUI api and View widget api
     *  \details widget_adapter translate basic event and pixel coordinate into widget event and coordinates
     */
    class widget_adapter : private display_controler {
    public:
        /**
         *  \param root the widget that will be displayed
         *  \param pixel_per_unit the number of pixel that must be used to render one widget unit
         */
        widget_adapter(widget& root, float pixel_per_unit);
        virtual ~widget_adapter() = default;

        /**
         *  \return display width in pixel
         */
        unsigned int display_width()    { return _display_width; }

        /**
         *  \return display height in pixel
         */
        unsigned int display_height()   { return _display_height; }

        /**
         *  \brief Notify the widget that the display is resized
         */
        void resize_display(unsigned int width, unsigned int height);

        /**
         *  'Low level' callback, called by system event processing
         *  and translated into 'higher level' events.
         *
         *  Coordinate are given in pixel.
         */
        void sys_draw(cairo_t *cr);
        void sys_draw_rect(
            cairo_t *cr, unsigned int top, unsigned int bottom,
            unsigned int left, unsigned int right);

		bool sys_mouse_move(unsigned int cx, unsigned int cy);
		bool sys_mouse_enter(void);
		bool sys_mouse_exit(void);
		bool sys_mouse_button_down(const mouse_button button);
		bool sys_mouse_button_up(const mouse_button button);
		bool sys_mouse_wheel(const float distance);
		bool sys_mouse_dbl_click(void);
		bool sys_key_down(const keycode key);
		bool sys_key_up(const keycode key);

    protected:
        /**
         *  \brief Inform the underlying display that an area must be redrawn
         */
        using draw_area = rectangle<int>;
        virtual void sys_invalidate_rect(const draw_area& area) =0;

    private:
        /**
         *  Drawing helpers
         */
        void begin_drawing(cairo_t *cr);
        void finish_drawing(cairo_t *cr);

        /**
         *  Display controler feature (called by root widget)
         */
        void invalidate_widget() override;
        void invalidate_rect(const rectangle<>& rect) override;
        float widget_pos_x() override;
        float widget_pos_y() override;
        /**
         *  Display / Widget coordinate translation
         */
        void _coord_display2widget(int x, int y, float &fx, float &fy);
        void _coord_widget2display(float fx, float fy, int &x, int &y);

        widget& _root;
        unsigned int _display_width;
        unsigned int _display_height;
        float _pixel_per_unit;

        float _cursor_fx{0.f};
        float _cursor_fy{0.f};
        bool _is_draging{false};
        mouse_button _draging_button;
        unsigned int _pressed_button_count{0u};
    };

}

#endif