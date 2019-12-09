
#include "view.h"



int main()
{
    using namespace View;

    simple_panel<> panel{100, 100};

    native_application_display dpy{panel, 6};

    panel.insert_widget(12.2, 38, std::make_unique<simple_panel<>>(15, 18));
    panel.insert_widget(20, 60, std::make_unique<simple_panel<>>(15, 18));

    dpy.open("");
    dpy.wait();

    return 0;
}