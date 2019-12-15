#ifndef VIEW_H_
#define VIEW_H_

//  TODO multiplatform settup
#include "display/implementations/x11_application_display.h"
#include "widget/widget.h"
#include "widget/widget_container.h"

//  Controls
#include "controls/simple_panel.h"
#include "controls/push_button.h"
#include "controls/checkbox.h"
#include "controls/knob.h"

namespace View {

    using native_application_display = x11_application_display;

}

#endif