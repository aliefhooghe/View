
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

# Use Freetype
find_package(Freetype REQUIRED)

add_library(nanovg ${NANOVG_SRC})
target_include_directories(nanovg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src ${FREETYPE_INCLUDE_DIRS})
target_compile_definitions(nanovg PUBLIC FONS_USE_FREETYPE)
target_link_libraries(nanovg INTERFACE ${FREETYPE_LIBRARIES})

set_target_properties(nanovg PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (WIN32)
    target_compile_definitions(nanovg PUBLIC "_CRT_SECURE_NO_WARNINGS")
endif()

