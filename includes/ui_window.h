#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include "connect4.h"
#include "ui_game.h"

typedef struct s_window_context
{
    Display *display;
    int screen;
    Window root;
    Window window;
    cairo_surface_t *surface;
    cairo_t *cr;
    Atom wmDeleteWindow;
    void *memory;
    Clay_Arena arena;
    char *fonts[2];  // Fixed size array for our two fonts
    int width;
    int height;
    bool initialized;  // Track initialization state
} t_window_context;

// Window management functions
t_window_context *init_window(t_game *game);
void cleanup_window(t_window_context *ctx);

#endif