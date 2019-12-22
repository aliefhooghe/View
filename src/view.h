#ifndef VIEW_H_
#define VIEW_H_

//  TODO multiplatform settup
#include "display/implementations/x11_application_display.h"
#include "widget/widget.h"

//  Widget Container
#include "widget_container/color_panel.h"
#include "widget_container/pair_layout.h"
#include "widget_container/header.h"
#include "widget_container/background.h"

//  Controls
#include "controls/push_button.h"
#include "controls/checkbox.h"
#include "controls/knob.h"

namespace View {

    using native_application_display = x11_application_display;

}

#endif