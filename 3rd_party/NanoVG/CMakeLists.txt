
cmake_minimum_required (VERSION 3.1)

project(NanoVG C)

# NanoVG library

set(NANOVG_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/fontstash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/nanovg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/nanovg_gl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/nanovg_gl_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/nanovg.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/stb_image.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/stb_truetype.h
)

add_library(nanovg ${NANOVG_SRC})

# Use Freetype
find_package(Freetype REQUIRED)
target_include_directories(nanovg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src)
target_compile_definitions(nanovg PUBLIC FONS_USE_FREETYPE)
target_link_libraries(nanovg PUBLIC Freetype::Freetype)

# Allow use in a shared library
set_target_properties(nanovg PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Preconfigure Target for use with View : Use OpenGL2
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
target_compile_definitions(nanovg INTERFACE NANOVG_GLEW NANOVG_GL2_IMPLEMENTATION)
target_link_libraries(nanovg INTERFACE OpenGL::GL GLEW::GLEW)
		
if (WIN32)
    target_compile_definitions(nanovg PUBLIC "_CRT_SECURE_NO_WARNINGS")
endif()

