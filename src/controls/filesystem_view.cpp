
#include "filesystem_view.h"

namespace View
{
    filesystem_view::filesystem_view(const std::filesystem::path& path, float width, float height)
    :    owning_directory_view<filesystem_directory_model>(
            std::make_unique<filesystem_directory_model>(path),
            width, height)
    {
    }

    void filesystem_view::update()
    {
        data_model().sync();
        owning_directory_view<filesystem_directory_model>::update();
    }


}

