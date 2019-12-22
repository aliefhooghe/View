
#include <iostream>

#include "view.h"

int main()
{
    using namespace View;

    auto left_panel = std::make_unique<panel<>>(100, 100);
    left_panel->insert_widget(5, 5, std::make_unique<checkbox>(3));
    left_panel->insert_widget(5, 15, std::make_unique<push_button>(16, 5, "Activate"));
    left_panel->insert_widget(5, 25, std::make_unique<knob>(8));

    auto content =
        std::make_unique<horizontal_pair_layout>(
            std::make_unique<header>(std::move(left_panel)),
            std::make_unique<vertical_pair_layout>(
                    std::make_unique<header>(std::make_unique<panel<>>(50, 20, free_size, size_constraint{15, 30})),
                    std::make_unique<header>(std::make_unique<panel<>>(50, 50))));

    auto root =
        std::make_unique<background>(std::move(content));

    //
    native_application_display dpy{*root, 6};
    dpy.open("");
    dpy.wait();

    std::cout << "Demo finnishing" << std::endl;
    return 0;
}