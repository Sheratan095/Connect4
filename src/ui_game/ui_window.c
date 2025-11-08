#include "connect4.h"
#include "ui_window.h"
#include "ui_game.h"

t_window_context *init_window(t_game *game)
{
	t_window_context *ctx = malloc(sizeof(t_window_context));
	if (!ctx)
		return NULL;

	// Initialize all fields to 0/NULL
	memset(ctx, 0, sizeof(t_window_context));

	// Open display first to get screen dimensions
	ctx->display = XOpenDisplay(NULL);
	if (!ctx->display)
	{
		ft_printf("Cannot open display\n");
		free(ctx);
		return NULL;
	}

	ctx->screen = DefaultScreen(ctx->display);
	ctx->root = RootWindow(ctx->display, ctx->screen);

	// Get screen dimensions
	int screen_width = DisplayWidth(ctx->display, ctx->screen);
	int screen_height = DisplayHeight(ctx->display, ctx->screen);

	// Calculate window size based on board dimensions
	// Add padding: title (100px), instructions (60px), margins (40px)
	int board_width = game->cols * CELL_SIZE;
	int board_height = game->rows * CELL_SIZE;
	int padding_horizontal = 40; // 20px on each side
	int padding_vertical = 200;	 // Title + instructions + margins

	ctx->width = board_width + padding_horizontal;
	ctx->height = board_height + padding_vertical;

	// Ensure minimum window size
	if (ctx->width < 400)
		ctx->width = 400;
	if (ctx->height < 500)
		ctx->height = 500;

	// Constrain to 90% of screen size (leave room for window decorations)
	int max_width = (screen_width * 9) / 10;
	int max_height = (screen_height * 9) / 10;

	if (ctx->width > max_width)
		ctx->width = max_width;
	if (ctx->height > max_height)
		ctx->height = max_height;

	ctx->initialized = false;

	// Initialize fonts
	ctx->fonts[0] = "Sans";
	ctx->fonts[1] = "Sans";

	// Center window on screen
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

	// Set the surface to not reference the display after cleanup
	// This prevents Cairo from caching screen info that leaks
	cairo_xlib_surface_set_size(ctx->surface, ctx->width, ctx->height);

	ctx->cr = cairo_create(ctx->surface);
	if (!ctx->cr)
	{
		ft_printf("Failed to create Cairo context\n");
		cleanup_window(ctx);
		return NULL;
	}

	// Set up Clay
	Clay_Cairo_Initialize(ctx->cr);

	// Calculate memory needed based on board size
	// Each cell needs 1 Clay element (optimized from 2)
	// Each row needs 1 container element
	// Plus: 1 root, 1 board container, 3 text elements = ~5 base elements
	int32_t cellElements = game->rows * game->cols;															// 1 per cell
	int32_t rowElements = game->rows;																						// 1 per row
	int32_t baseElements = 5;																										// root + board + 3 texts
	int32_t estimatedElements = cellElements + rowElements + baseElements + 20; // Safety margin

	// Set maximum element count BEFORE Clay_Initialize
	// Add generous buffer to prevent out-of-bounds errors
	int32_t maxElementCount = estimatedElements * 6; // Increased to 6x (was 3x)
	if (maxElementCount < 1024)
		maxElementCount = 1024; // Increased minimum (was 512)

	Clay_SetMaxElementCount(maxElementCount);
	ft_printf("Clay max elements set to: %d (estimated %d for %dx%d board)\n",
						maxElementCount, estimatedElements, game->rows, game->cols);

	size_t bytesPerElement = 512; // Increased from 256
	size_t requiredMemory = maxElementCount * bytesPerElement;

	// Ensure we have at least Clay_MinMemorySize()
	size_t minMemorySize = Clay_MinMemorySize();
	if (requiredMemory < minMemorySize)
		requiredMemory = minMemorySize;

	// Add 50% buffer for render commands and text caching
	requiredMemory = (requiredMemory * 3) / 2;

	ft_printf("Allocating %zu bytes for Clay memory (for %d max elements)\n",
						requiredMemory, maxElementCount);

	// Use aligned_alloc for proper 64-byte alignment (cacheline) required by Clay
	// Clay's Clay_Initialize() expects 64-byte aligned memory
	size_t alignment = 64;
	size_t alignedSize = ((requiredMemory + alignment - 1) / alignment) * alignment;

	ctx->memory = ft_calloc(1, alignedSize);
	if (!ctx->memory)
	{
		ft_printf("Failed to allocate Clay memory\n");
		cleanup_window(ctx);
		return NULL;
	}

	ctx->arena = Clay_CreateArenaWithCapacityAndMemory(alignedSize, ctx->memory);

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
		// Clean up Cairo resources BEFORE closing X display
		// Cairo needs the display to properly free X11 surface resources
		if (ctx->cr)
		{
			cairo_destroy(ctx->cr);
			ctx->cr = NULL;
		}
		if (ctx->surface)
		{
			// Finish and flush the surface before destroying
			cairo_surface_finish(ctx->surface);
			cairo_surface_destroy(ctx->surface);
			ctx->surface = NULL;
		}

		// Force Cairo to clean up all cached resources
		cairo_debug_reset_static_data();

		// NOW close X11 resources after Cairo is done
		if (ctx->display)
		{
			// First destroy the window if it exists
			if (ctx->window)
				XDestroyWindow(ctx->display, ctx->window);

			// Then close the display
			XCloseDisplay(ctx->display);
		} // Clean up Clay resources
		if (ctx->memory)
		{
			free(ctx->memory);
			ctx->memory = NULL;
		}
	}

	// Finally free the context itself
	free(ctx);
}