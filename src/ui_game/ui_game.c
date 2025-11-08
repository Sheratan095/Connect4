#include "connect4.h"  // Move connect4.h first so its dependencies are loaded in correct order

Clay_Color getPlayerColor(char player)
{
	if (player == '1')
		return (Clay_Color){220, 50, 50, 255};
	if (player == '2')
		return (Clay_Color){220, 200, 50, 255};
	return (Clay_Color){250, 250, 250, 255};
}

void createUI(t_game *game)
{
	Clay_BeginLayout();
	// Root
	CLAY_AUTO_ID({.layout = {
					  .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
					  .layoutDirection = CLAY_TOP_TO_BOTTOM,
					  .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER},
					  .childGap = 16},
				  .backgroundColor = {30, 30, 40, 255}})
	{
		// Title
		CLAY_TEXT(CLAY_STRING("Connect 4"), CLAY_TEXT_CONFIG({.fontSize = 40, .textColor = {255, 255, 255, 255}}));
		// Status
		const char *status = game->game_over
			? (game->winner == 0 ? "Draw" : (game->winner == PLAYER ?  "You won!" : "AI won!"))
			: (game->current_player == PLAYER ? "Your turn" : "AI's Turn");
		Clay_Color statusColor = game->game_over && game->winner != 0 ? getPlayerColor(game->winner) : getPlayerColor(game->current_player);
		Clay_String statusString = {
			.isStaticallyAllocated = FALSE,
			.length = (int32_t)strlen(status),
			.chars = status
		};
		CLAY_TEXT(statusString, CLAY_TEXT_CONFIG({
			.fontSize = 24,
			.textColor = statusColor
		}));
		// Board
		CLAY_AUTO_ID({.layout = {
						  .sizing = {CLAY_SIZING_FIXED(BOARD_COLS * CELL_SIZE), CLAY_SIZING_FIXED(BOARD_ROWS * CELL_SIZE)},
						  .layoutDirection = CLAY_TOP_TO_BOTTOM},
					  .backgroundColor = {40, 80, 160, 255},
					  .cornerRadius = CLAY_CORNER_RADIUS(12)})
		{
			for (int row = 0; row < BOARD_ROWS; row++)
			{
				CLAY_AUTO_ID({.layout = {
								  .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(CELL_SIZE)},
								  .layoutDirection = CLAY_LEFT_TO_RIGHT}})
				{
					for (int col = 0; col < BOARD_COLS; col++)
					{
						Clay_Color cellColor = getPlayerColor(game->board[row][col]);
						bool isLast = (row == game->last_row && col == game->last_col);
						CLAY_AUTO_ID({.layout = {
										  .sizing = {CLAY_SIZING_FIXED(CELL_SIZE), CLAY_SIZING_FIXED(CELL_SIZE)},
										  .padding = {6, 6, 6, 6}}})
						{
							CLAY_AUTO_ID({.layout = {.sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()}},
										  .backgroundColor = cellColor,
										  .cornerRadius = CLAY_CORNER_RADIUS(CELL_SIZE / 2 - 8),
										  .border = {.color = {255, 255, 255, 255}, .width = {.left = isLast ? 3 : 0, .right = isLast ? 3 : 0, .top = isLast ? 3 : 0, .bottom = isLast ? 3 : 0, .betweenChildren = 0}}}) {}
						}
					}
				}
			}
		}
		// Instructions
		const char *instr = game->game_over ? "Click to restart" : "Click a column to drop";
		Clay_String instrString = {
			.isStaticallyAllocated = FALSE,
			.length = (int32_t)strlen(instr),
			.chars = instr
		};
		CLAY_TEXT(instrString, CLAY_TEXT_CONFIG({
			.fontSize = 18,
			.textColor = {180, 180, 180, 255}
		}));
	}
	Clay_EndLayout();
}

void HandleClayErrors(Clay_ErrorData errorData)
{
	printf("Clay Error: %s\n", errorData.errorText.chars);
}

void start_game_ui(t_game *game)
{
	Display *display = XOpenDisplay(NULL);
	if (!display)
	{
		fprintf(stderr, "Cannot open display\n");
		return;
	}
	int screen = DefaultScreen(display);
	Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, BlackPixel(display, screen), BlackPixel(display, screen));
	XStoreName(display, window, "Connect 4 - Clay UI");
	XSelectInput(display, window, ExposureMask | ButtonPressMask);
	XMapWindow(display, window);
	cairo_surface_t *surface = cairo_xlib_surface_create(display, window, DefaultVisual(display, screen), WINDOW_WIDTH, WINDOW_HEIGHT);
	cairo_t *cr = cairo_create(surface);
	Clay_Cairo_Initialize(cr);
	uint64_t totalMemorySize = Clay_MinMemorySize();
	Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, malloc(totalMemorySize));
	Clay_Initialize(arena, (Clay_Dimensions){WINDOW_WIDTH, WINDOW_HEIGHT}, (Clay_ErrorHandler){HandleClayErrors, NULL});
	char **fonts = (char *[]){"Sans", "Sans"};
	Clay_SetMeasureTextFunction(Clay_Cairo_MeasureText, fonts);
	bool running = TRUE;
	bool needsRedraw = TRUE;
	const struct timespec frameDelay = {
		.tv_sec = 0,
		.tv_nsec = 16 * 1000 * 1000
	};
	while (running)
	{
		while (XPending(display))
		{
			XEvent event;
			XNextEvent(display, &event);
			if (event.type == Expose)
				needsRedraw = TRUE;
			else if (event.type == ButtonPress && event.xbutton.button == Button1)
			{
				if (check_endgame(game))
				{
					game = init_new_game(BOARD_ROWS, BOARD_COLS);
				}
				else
				{
					int col = (event.xbutton.x - (WINDOW_WIDTH - BOARD_COLS * CELL_SIZE) / 2) / CELL_SIZE;
					insert_pawn(game, col);
				}
				needsRedraw = TRUE;
			}
		}
		if (needsRedraw)
		{
			createUI(game);
			Clay_RenderCommandArray commands = Clay_EndLayout();
			Clay_Cairo_Render(commands, fonts);
			cairo_surface_flush(surface);
			XFlush(display);
			needsRedraw = FALSE;
		}
		nanosleep(&frameDelay, NULL);
	}

	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	XCloseDisplay(display);
}