
#include <iostream>

#include "view.h"

int main()
{
    using namespace View;

    simple_panel<> panel{100, 100};

    native_application_display dpy{panel, 6};

    auto button = std::make_unique<push_button>(15, 4);
    button->set_callback([](auto&) { std::cout << "Pushed !" << std::endl; });
    panel.insert_widget(5, 30, std::move(button));

    panel.insert_widget(5, 5, std::make_unique<checkbox>(3));

    dpy.open("");
    dpy.wait();

    return 0;
}