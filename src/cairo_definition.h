#ifndef VIEW_CAIRO_DEFINITION_H_
#define VIEW_CAIRO_DEFINITION_H_

#ifdef _WIN32
#define CAIRO_WIN32_STATIC_BUILD
#define NOMINMAX
#include <cairo.h>
#include <cairo-win32.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <cairo.h>
#include <cairo-xlib.h>
#endif

#endif