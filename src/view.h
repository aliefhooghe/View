#ifndef VIEW_H_
#define VIEW_H_

//  TODO multiplatform settup
#include "display/implementations/x11_application_display.h"
#include "display/implementations/x11_vst2_display.h"
#include "widget/widget.h"
#include "widget/widget_proxy.h"

//  Widget Container
#include "widget_container/panel.h"
#include "widget_container/pair_layout.h"
#include "widget_container/header.h"
#include "widget_container/background.h"
#include "widget_container/map_wrapper.h"

//  Controls
#include "controls/label.h"
#include "controls/push_button.h"
#include "controls/text_input.h"
#include "controls/text_push_button.h"
#include "controls/checkbox.h"
#include "controls/knob.h"
#include "controls/directory_view.h"

namespace View {

    using native_application_display = x11_application_display;
    using native_vst2_display = x11_vst2_display;

}

#endif