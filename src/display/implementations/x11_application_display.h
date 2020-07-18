#ifndef VIEW_X11_APPLICATION_DISPLAY_H_
#define VIEW_X11_APPLICATION_DISPLAY_H_

#include "display/display_types/application_display.h"
#include "../backends/x11_display.h"

namespace View {

    class x11_application_display : public application_display, private x11_display {
    public:
        x11_application_display(widget& root, float pixel_per_unit);
        ~x11_application_display();

        void open(const std::string& title) override;
        void wait() override;
        void close() override;
        bool is_open() override;
    };

}

#endif