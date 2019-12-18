
#include <iostream>

#include "view.h"

int main()
{
    using namespace View;

    color_panel<> panel{100, 100, free_size, free_size};
    native_application_display dpy{panel, 6};

    panel.insert_widget(5, 5, std::make_unique<checkbox>(3));
    auto button = std::make_unique<push_button>(15, 4);
    button->set_callback([](auto&) { std::cout << "Pushed !" << std::endl; });
    panel.insert_widget(5, 15, std::move(button));

    panel.insert_widget(5, 25, std::make_unique<knob>(8));

    dpy.open("");
    dpy.wait();

    return 0;
}