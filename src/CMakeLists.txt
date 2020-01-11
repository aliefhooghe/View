cmake_minimum_required (VERSION 3.1)

# include guard
if(TARGET View)
	return()
endif()

project(View CXX)
set(CMAKE_CXX_STANDARD 17)

############################
#                          #
#       Build Options      #
#                          #
############################

option(VIEW_X11_BACKEND "" ON)

############################
#                          #
#       VIEW LIBRARY       #
#                          #
############################


## Dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
find_package(Cairo REQUIRED)
find_package(PangoCairo REQUIRED)

## Common Sources
set(VIEW_COMMON_SRC
    cairo_definition.h
    view.h

    controls/control.h
    controls/control.cpp
    controls/checkbox.h
    controls/checkbox.cpp
    controls/knob.h
    controls/knob.cpp
    controls/push_button.h
    controls/push_button.cpp
    controls/text_push_button.cpp
    controls/text_push_button.h

    display/display_types/application_display.h
    display/common/display_controler.h
    display/common/widget_adapter.cpp
    display/common/widget_adapter.h

    drawing/cairo_helper.h
    drawing/cairo_helper.cpp
    drawing/color.h
    drawing/text_helper.h
    drawing/text_helper.cpp
    drawing/named_colors.h

    helpers/directory_model.h

    widget/event.h
    widget/widget.cpp
    widget/widget.h
    widget/rectangle.h
    widget/size_constraint.h
    widget/orientation.h

    widget_container/widget_container.h
    widget_container/background.h
    widget_container/background.cpp
    widget_container/layout_separator.h
    widget_container/layout_separator.cpp
    widget_container/header.h
    widget_container/header.cpp
    widget_container/panel.h
    widget_container/color_panel.h
    widget_container/pair_layout.h
    helpers/alphabetical_compare.h)

##  View Library
add_library(View STATIC ${VIEW_COMMON_SRC})
target_include_directories(View PUBLIC ${Cairo_INCLUDE_DIRS} ${PangoCairo_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(View PUBLIC ${Cairo_LIBRARIES} ${PangoCairo_LIBRARIES})

##  X11 Backend
if (VIEW_X11_BACKEND)
    find_package(X11 REQUIRED)
    find_package(Xext REQUIRED)

    target_sources(View PRIVATE
        display/backends/x11_display.cpp
        display/backends/x11_display.h
        display/implementations/x11_application_display.cpp
        display/implementations/x11_application_display.h
    )

	target_include_directories(View PUBLIC
		${X11_INCLUDE_DIR}
		${Xext_INCLUDE_DIR}
	)

	target_link_libraries(View PRIVATE
		${X11_LIBRARIES}
		${Xext_LIBRARY}
		# stdc++fs			# filesystem
        pthread
	)
endif()

#####################
#                   #
#       TESTS       #
#                   #
#####################

# widgets_demo
add_executable(widgets_demo Tests/widgets_demo.cpp)
target_link_libraries(widgets_demo PUBLIC View)
