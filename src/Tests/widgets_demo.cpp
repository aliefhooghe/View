
#include <iostream>

#include "view.h"
#include "helpers/layout_builder.h"

int main()
{
    // tree structure
    auto dir_view = std::make_unique<View::filesystem_view>("/home", 250, 250);

    dir_view->set_value_select_callback([](const auto& file) { std::cout << "Select file " << file << std::endl; });
    dir_view->set_directory_select_callback([](const auto& dir) { std::cout << dir.path() << std::endl; });

    auto update_button = std::make_unique<View::text_push_button>("Update");
    update_button->set_callback([dv = dir_view.get()]() { dv->update(); });

    View::layout_builder builder{};

    auto root =
        builder.windows(
            builder.horizontal<false>(
                builder.header(
                    builder.vertical(
                        builder.horizontal(std::make_unique<View::knob>(), builder.empty_space()),
                        builder.empty_space(),
                        builder.horizontal(std::make_unique<View::text_push_button>("Save"), std::move(update_button)),
                        std::make_unique<View::text_input>()
                    )
                ),
                builder.header(
                    std::move(dir_view)
                )
            )
        );

    //
    auto dpy = create_application_display(*root, 1);
    dpy->open("");
    dpy->wait();

    std::cout << "Demo finnishing" << std::endl;

    return 0;
}