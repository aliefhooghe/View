
#include <iostream>

#include "view.h"
#include "helpers/filesystem_directory_model.h"

int main()
{
    using namespace View;

    // tree structure
    filesystem_directory_model model{"/home/aliefhooghe"};

    auto dir_view = make_directory_view(model, 280, 280);

    dir_view->set_value_select_callback([](const auto& file) { std::cout << "Select file " << file << std::endl; });
    dir_view->set_directory_select_callback([](const auto& dir) { std::cout << dir.path() << std::endl; });

    auto update_button = std::make_unique<text_push_button>("Update");
    update_button->set_callback([dv = dir_view.get()]() { dv->update(); });

    auto left_panel = std::make_unique<panel<>>(280, 280, size_constraint{140, 1500}, free_size);
    left_panel->insert_widget(70, 14, std::make_unique<knob>());
    left_panel->insert_widget(168, 14, std::make_unique<text_push_button>("Save"));
    left_panel->insert_widget(168, 42, std::move(update_button));
    left_panel->insert_widget(70, 112, std::make_unique<text_input>());

    for (auto i = 0u; i < 15u; ++i) {
        const float y = static_cast<float>(i) * 28.f + 140;
        left_panel->insert_widget(42, y, std::make_unique<checkbox>());
    }

    auto content =
        std::make_unique<horizontal_pair_layout>(
            std::make_unique<header>(std::move(left_panel)),
            std::make_unique<vertical_pair_layout>(
                    make_horizontal_layout(
                        std::make_unique<header>(std::make_unique<panel<>>(280, 140, free_size, size_constraint{42, 210})),
                        std::make_unique<header>(std::make_unique<panel<>>(280, 140, free_size, size_constraint{42, 210})),
                        std::make_unique<header>(std::make_unique<panel<>>(280, 140, free_size, size_constraint{42, 210}))
                    ),
                    std::make_unique<header>(std::move(dir_view))));

    auto root =
        std::make_unique<background>(std::move(content));

    //
    auto dpy = create_application_display(*root, 1);
    dpy->open("");
    dpy->wait();

    std::cout << "Demo finnishing" << std::endl;

    return 0;
}