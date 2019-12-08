
#include "view.h"



int main()
{
    using namespace View;

    simple_panel<> panel{100, 100};

    panel.insert_widget(12.2, 38, std::make_unique<simple_panel<>>(15, 18));

    native_application_display dpy{panel, 10};

    dpy.open("");
    dpy.wait();

    return 0;
}