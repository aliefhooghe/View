
#include <iostream>

#include "view.h"

int main()
{
    using namespace View;

    auto left_panel = std::make_unique<color_panel<>>(100, 100, size_constraint{50, 150}, free_size);
    left_panel->insert_widget(5, 5, std::make_unique<checkbox>(3));
    left_panel->insert_widget(5, 15, std::make_unique<push_button>(15, 4));
    left_panel->insert_widget(5, 25, std::make_unique<knob>(8));

    auto root =
        std::make_unique<horizontal_pair_layout>(
            std::move(left_panel),
            std::make_unique<horizontal_pair_layout>(
                std::make_unique<vertical_pair_layout>(
                    std::make_unique<color_panel<>>(50, 50,  size_constraint{50, 75}, free_size, 0x00FF00FFu),
                    std::make_unique<color_panel<>>(50, 50, free_size, free_size, 0x00FFE0FFu)),
                std::make_unique<color_panel<>>(50, 50, free_size, free_size, 0xFF00FFFFu)
            ));

    //
    native_application_display dpy{*root, 6};
    dpy.open("");
    dpy.wait();

    std::cout << "Demo finnishing" << std::endl;
    return 0;
}