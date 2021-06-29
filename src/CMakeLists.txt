cmake_minimum_required (VERSION 3.1)

# include guard
if(TARGET View)
	return()
endif()

project(View CXX)
set(CMAKE_CXX_STANDARD 17)

############################
#                          #
#       VIEW LIBRARY       #
#                          #
############################


## Dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/NanoVG NanoVG)

## Common Sources
set(VIEW_COMMON_SRC
    view.h
    view.cpp

    controls/control.h
    controls/control.cpp
    controls/checkbox.h
    controls/checkbox.cpp
    controls/directory_view.h
    controls/knob.h
    controls/knob.cpp
    controls/label.h
    controls/label.cpp
    controls/push_button.h
    controls/push_button.cpp
    controls/text_input.h
    controls/text_input.cpp
    controls/text_push_button.cpp
    controls/text_push_button.h

    display/backends/view_backend.h
    display/common/display_controler.h
    display/common/widget_adapter.cpp
    display/common/widget_adapter.h
    display/frontends/application_display.cpp
    display/frontends/application_display.h
    display/frontends/vst2_display.cpp
    display/frontends/vst2_display.h

    drawing/text_helper.h
    drawing/text_helper.cpp
    drawing/shadowed.h
    drawing/shadowed.cpp

    helpers/alphabetical_compare.h
    helpers/directory_model.h
    helpers/filesystem_directory_model.h
    helpers/filesystem_directory_model.cpp
    helpers/layout_builder.h
    helpers/layout_builder.cpp
    controls/filesystem_view.h
    controls/filesystem_view.cpp

    internal_fonts/internal_fonts_data.cpp
    internal_fonts/internal_fonts.cpp
    internal_fonts/internal_fonts.h

    widget/event.h
    widget/color_theme.h
    widget/widget.cpp
    widget/widget.h
    widget/widget_proxy.h
    widget/rectangle.h
    widget/size_constraint.h
    widget/orientation.h

    widget_container/widget_container.h
    widget_container/background.h
    widget_container/background.cpp
    widget_container/border_wrapper.h
    widget_container/border_wrapper.cpp
    widget_container/layout_separator.h
    widget_container/layout_separator.cpp
    widget_container/map_wrapper.h
    widget_container/map_wrapper.cpp
    widget_container/header.h
    widget_container/header.cpp
    widget_container/panel.h
    widget_container/pair_layout.h
    widget_container/widget_wrapper_base.h

    helpers/alphabetical_compare.h
)

##  View Library
add_library(View STATIC ${VIEW_COMMON_SRC})
set_target_properties(View PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(View PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(View PUBLIC nanovg)

##  X11 Backend for UNIX
if (UNIX)
    message("Build View with X11 backend")

    find_package(X11 REQUIRED)

    target_sources(View PRIVATE
        display/backends/x11_backend.cpp
        display/backends/x11_backend.h
    )

	target_link_libraries(View PRIVATE
		${X11_LIBRARIES}
		# stdc++fs			# filesystem
	)
elseif (WIN32)
    message("Build View with Win32 backend")

    target_sources(View PRIVATE
        display/backends/win32_backend.cpp
        display/backends/win32_backend.h)

    target_compile_definitions(View PRIVATE _USE_MATH_DEFINES)
endif()

#####################
#                   #
#       TESTS       #
#                   #
#####################

# widgets_demo
add_executable(widgets_demo Tests/widgets_demo.cpp)
target_link_libraries(widgets_demo PUBLIC View)
