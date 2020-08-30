#ifndef VIEW_SHADOWED_H_
#define VIEW_SHADOWED_H_

#include <nanovg.h>

namespace View {

    void shadowed_down_rounded_rect(
        NVGcontext *vg,
        float x, float y, float width, float height, float radius,
        NVGcolor background, NVGcolor surface);

    void shadowed_up_rounded_rect(
        NVGcontext *vg,
        float x, float y, float width, float height, float radius,
        NVGcolor background, NVGcolor surface);

}

#endif /* VIEW_SHADOWED_H_ */