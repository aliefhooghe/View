#ifndef VIEW_FILESYSTEM_VIEW_H_
#define VIEW_FILESYSTEM_VIEW_H_

#include "../helpers/filesystem_directory_model.h"
#include "directory_view.h"

namespace View
{
    class filesystem_view : public owning_directory_view<filesystem_directory_model>
    {
    public:
        filesystem_view(const std::filesystem::path& path, float width, float height);
        ~filesystem_view() override = default;

        void update() override;
    };
}

#endif /* VIEW_FILESYSTEM_VIEW_H_ */