#pragma once

#include "clay.h"
#include <cairo/cairo.h>

void Clay_Cairo_Initialize(cairo_t *cairo);
void Clay_Cairo_Render(Clay_RenderCommandArray commands, char** fonts);
void Clay_Cairo__Blit_Surface(cairo_surface_t *src_surface, cairo_surface_t *dest_surface,
                             double x, double y, double scale_x, double scale_y);
Clay_Dimensions Clay_Cairo_MeasureText(Clay_StringSlice str, Clay_TextElementConfig *config, void *userData);