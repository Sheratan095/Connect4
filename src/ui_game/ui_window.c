#include "connect4.h"
#include "ui_window.h"
#include "ui_game.h"

t_window_context *init_window(void)
{
    t_window_context *ctx = malloc(sizeof(t_window_context));
    if (!ctx)
        return NULL;
    
    // Initialize all fields to 0/NULL
    memset(ctx, 0, sizeof(t_window_context));
    
    // Set basic properties
    ctx->width = WINDOW_WIDTH;
    ctx->height = WINDOW_HEIGHT;
    ctx->initialized = false;
    
    // Initialize fonts
    ctx->fonts[0] = "Sans";
    ctx->fonts[1] = "Sans";
    
    // Open display
    ctx->display = XOpenDisplay(NULL);
    if (!ctx->display)
    {
        ft_printf("Cannot open display\n");
        cleanup_window(ctx);
        return NULL;
    }

    ctx->screen = DefaultScreen(ctx->display);
    ctx->root = RootWindow(ctx->display, ctx->screen);
    
    // Center window on screen
    int screen_width = DisplayWidth(ctx->display, ctx->screen);
    int screen_height = DisplayHeight(ctx->display, ctx->screen);
    int x = (screen_width - ctx->width) / 2;
    int y = (screen_height - ctx->height) / 2;
    
    // Create window
    ctx->window = XCreateSimpleWindow(ctx->display, ctx->root, x, y, ctx->width, ctx->height,
        0, BlackPixel(ctx->display, ctx->screen), BlackPixel(ctx->display, ctx->screen));
    
    if (!ctx->window)
    {
        ft_printf("Failed to create window\n");
        cleanup_window(ctx);
        return NULL;
    }
    
    XStoreName(ctx->display, ctx->window, "Connect 4");
    XSelectInput(ctx->display, ctx->window, ExposureMask | ButtonPressMask | StructureNotifyMask);
    
    // Set up window close handling
    ctx->wmDeleteWindow = XInternAtom(ctx->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(ctx->display, ctx->window, &ctx->wmDeleteWindow, 1);
    
    XMapWindow(ctx->display, ctx->window);
    
    // Set up Cairo
    ctx->surface = cairo_xlib_surface_create(ctx->display, ctx->window,
        DefaultVisual(ctx->display, ctx->screen), ctx->width, ctx->height);
    if (!ctx->surface)
    {
        ft_printf("Failed to create Cairo surface\n");
        cleanup_window(ctx);
        return NULL;
    }
    
    ctx->cr = cairo_create(ctx->surface);
    if (!ctx->cr)
    {
        ft_printf("Failed to create Cairo context\n");
        cleanup_window(ctx);
        return NULL;
    }
    
    // Set up Clay
    Clay_Cairo_Initialize(ctx->cr);
    ctx->memory = malloc(Clay_MinMemorySize());
    if (!ctx->memory)
    {
        ft_printf("Failed to allocate Clay memory\n");
        cleanup_window(ctx);
        return NULL;
    }
    
    ctx->arena = Clay_CreateArenaWithCapacityAndMemory(Clay_MinMemorySize(), ctx->memory);
    Clay_Initialize(ctx->arena, (Clay_Dimensions){ctx->width, ctx->height},
        (Clay_ErrorHandler){HandleClayErrors, NULL});
    
    Clay_SetMeasureTextFunction(Clay_Cairo_MeasureText, ctx->fonts);
    
    ctx->initialized = true;
    return ctx;
}

void cleanup_window(t_window_context *ctx)
{
    if (!ctx)
        return;
    
    if (ctx->initialized)
    {
        if (ctx->display)
        {
            // First destroy the window if it exists
            if (ctx->window)
                XDestroyWindow(ctx->display, ctx->window);
                
            // Then close the display
            XCloseDisplay(ctx->display);
        }
        
        // Clean up Cairo resources
        if (ctx->cr)
            cairo_destroy(ctx->cr);
        if (ctx->surface)
            cairo_surface_destroy(ctx->surface);
            
        // Clean up Clay resources
        if (ctx->memory)
            free(ctx->memory);
    }
    
    // Finally free the context itself
    free(ctx);
}