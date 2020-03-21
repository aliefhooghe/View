
#include <iostream>

#include "view.h"
#include "helpers/filesystem_directory_model.h"

int main()
{
    using namespace View;

    auto left_panel = std::make_unique<panel<>>(20, 20, size_constraint{10, 150}, free_size);
    left_panel->insert_widget(5, 5, std::make_unique<checkbox>());
    left_panel->insert_widget(5, 15, std::make_unique<text_push_button>("Save"));
    left_panel->insert_widget(5, 25, std::make_unique<knob>());

    // tree structure
    filesystem_directory_model model{"/home/aliefhooghe"};

    auto dir_view = make_directory_view(model, 20, 20);

    auto content =
        std::make_unique<horizontal_pair_layout>(
            std::make_unique<header>(std::move(left_panel)),
            std::make_unique<vertical_pair_layout>(
                    std::make_unique<header>(std::make_unique<panel<>>(50, 20, free_size, size_constraint{15, 30})),
                    std::make_unique<header>(std::move(dir_view))));

    auto root =
        std::make_unique<background>(std::move(content));

    //
    native_application_display dpy{*root, 14};
    dpy.open("");
    dpy.wait();

    std::cout << "Demo finnishing" << std::endl;

    return 0;
}