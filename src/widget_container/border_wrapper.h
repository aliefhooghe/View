#ifndef VIEW_BORDER_WRAPER_H_
#define VIEW_BORDER_WRAPER_H_

#include "widget_wrapper_base.h"

namespace View {

    class border_wrapper : public widget_wrapper_base<border_wrapper> {
    public:
        border_wrapper(
            std::unique_ptr<widget>&& root,
            float border_top = 5.f,
            float border_bottom = 5.f,
            float border_left = 5.f,
            float border_right = 5.f);

        ~border_wrapper() override = default;
        bool resize(float width, float height) override;

    private:
        const float _border_top;
        const float _border_bottom;
        const float _border_left;
        const float _border_right;
    };

}

#endif
