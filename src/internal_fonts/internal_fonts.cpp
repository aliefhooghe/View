
#include <cstdint>

#include "internal_fonts.h"

/*
 * Reference to embeded data 
 */
extern unsigned char roboto_regular_ttf[];
extern unsigned int roboto_regular_ttf_len;
extern unsigned char roboto_bold_ttf[];
extern unsigned int roboto_bold_ttf_len;

namespace View {

    int create_roboto_regular_font(NVGcontext *vg)
    {
        return nvgCreateFontMem(vg, "", roboto_regular_ttf, roboto_regular_ttf_len, 0 /* do not free */);
    }

    int create_roboto_bold_font(NVGcontext *vg)
    {
        return nvgCreateFontMem(vg, "", roboto_bold_ttf, roboto_bold_ttf_len, 0);
    }

}