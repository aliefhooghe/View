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

## Common Sources
set(VIEW_COMMON_SRC
    cairo_definition.h
    controls/simple_panel.h
    display/display_types/application_display.h
    display/widget_adapter.cpp
    display/widget_adapter.h
    drawing/cairo_helper.h
    drawing/color.h
    Tests/widgets_demo.cpp
    view.h
    widget/event.h
    widget/widget_container.h
    widget/widget.cpp
    widget/widget.h
    widget/display_controler.h
)

## Find Cairo Graphics librarie
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
find_package(Cairo REQUIRED)

##  View Library
add_library(View STATIC ${VIEW_COMMON_SRC})
target_include_directories(View PUBLIC ${Cairo_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(View PUBLIC ${Cairo_LIBS})

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
