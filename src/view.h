#ifndef VIEW_H_
#define VIEW_H_

//  TODO multiplatform settup
#include "display/frontends/application_display.h"
#include "display/frontends/vst2_display.h"
#include "widget/widget.h"
#include "widget/widget_proxy.h"

//  Widget Container
#include "widget_container/panel.h"
#include "widget_container/pair_layout.h"
#include "widget_container/header.h"
#include "widget_container/background.h"
#include "widget_container/map_wrapper.h"
#include "widget_container/border_wrapper.h"

//  Controls
#include "controls/label.h"
#include "controls/filesystem_view.h"
#include "controls/push_button.h"
#include "controls/text_input.h"
#include "controls/text_push_button.h"
#include "controls/checkbox.h"
#include "controls/knob.h"
#include "controls/directory_view.h"

namespace View {

    std::unique_ptr<application_display> create_application_display(widget& root, float pixel_per_unit);
    std::unique_ptr<vst2_display> create_vst2_display(widget& root, float pixel_per_unit);

}

#endif