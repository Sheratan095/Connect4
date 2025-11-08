#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include "connect4.h"

Clay_Color	getPlayerColor(char player)
{
	switch ((int)player)
	{
		case PLAYER:  // PLAYER = 1
			return (Clay_Color){220, 0, 0, 255};      // Bright Red
		case AI:      // AI = 2
			return (Clay_Color){255, 215, 0, 255};    // Gold
		default:      // NULL_PLAYER = 0
			return (Clay_Color){230, 230, 230, 255};  // Light Grey
	}
}

static int	get_column_at_pos(int x, int y)
{
	// Calculate board position
	int	boardWidth = BOARD_COLS * CELL_SIZE;
	int	boardHeight = BOARD_ROWS * CELL_SIZE;
	int	boardX = (WINDOW_WIDTH - boardWidth) / 2;
	int	boardY = (WINDOW_HEIGHT - boardHeight) / 2 + 50;  // Account for title
	
	// First check if click is within board bounds
	if (x < boardX || x >= boardX + boardWidth || y < boardY || y >= boardY + boardHeight)
	{
		ft_printf("Click outside board area\n");
		return (-1);
	}
	
	// Calculate column
	int col = (x - boardX) / CELL_SIZE;
	
	// Validate column bounds (extra safety check)
	if (col < 0 || col >= BOARD_COLS)
	{
		ft_printf("Invalid column calculated: %d\n", col);
		return (-1);
	}
	
	ft_printf("Click at x=%d, y=%d (boardX=%d, boardY=%d), column=%d\n", x, y, boardX, boardY, col);
	return (col);
}

static void createUI(t_game *game)
{
	const char *status;
	const char *instr;
	Clay_Color statusColor;
	Clay_String statusString;
	Clay_String instrString;

	Clay_BeginLayout();
	
	// Root container
	CLAY_AUTO_ID({.layout = {
					 .sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()},
					 .layoutDirection = CLAY_TOP_TO_BOTTOM,
					 .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER},
					 .childGap = 16},
				 .backgroundColor = {30, 30, 40, 255}})
	{
		// Title
		CLAY_TEXT(CLAY_STRING("Connect 4"), 
			CLAY_TEXT_CONFIG({.fontSize = 40, .textColor = {255, 255, 255, 255}}));

		// Game status
		status = game->game_over
			? (game->winner == 0 ? "Draw!" : (game->winner == PLAYER ? "You won!" : "AI won!"))
			: (game->current_player == PLAYER ? "Your turn" : "AI is thinking...");
		
		statusColor = game->game_over && game->winner != 0 
			? getPlayerColor(game->winner) 
			: getPlayerColor(game->current_player);
		
		statusString.isStaticallyAllocated = false;
		statusString.length = strlen(status);
		statusString.chars = (char*)status;
		
		CLAY_TEXT(statusString, CLAY_TEXT_CONFIG({.fontSize = 24, .textColor = statusColor}));

		// Game board
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
						// Get cell value directly from board and pass to getPlayerColor
						int cellValue = game->board[row][col];
						Clay_Color cellColor = getPlayerColor(cellValue);
						bool isLast = (row == game->last_row && col == game->last_col);
						
						CLAY_AUTO_ID({.layout = {
										.sizing = {CLAY_SIZING_FIXED(CELL_SIZE), CLAY_SIZING_FIXED(CELL_SIZE)},
										.padding = {6, 6, 6, 6}}})
						{
							CLAY_AUTO_ID({.layout = {.sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_GROW()}},
										.backgroundColor = cellColor,
										.cornerRadius = CLAY_CORNER_RADIUS(CELL_SIZE / 2 - 8),
										.border = {
											.color = {255, 255, 255, 255},
											.width = {
												.left = isLast ? 3 : 0,
												.right = isLast ? 3 : 0,
												.top = isLast ? 3 : 0,
												.bottom = isLast ? 3 : 0
											}
										}}) {}
						}
					}
				}
			}
		}

		// Instructions
		instr = game->game_over ? "Click anywhere to restart" : "Click a column to place your piece";
		instrString.isStaticallyAllocated = false;
		instrString.length = strlen(instr);
		instrString.chars = (char*)instr;
		
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
	Display *display;
	int screen;
	Window root, window;
	cairo_surface_t *surface;
	cairo_t *cr;
	Atom wmDeleteWindow;
	void *memory;
	Clay_Arena arena;
	int screen_width, screen_height, x, y;
	bool running, needsRedraw;
	struct timespec frameDelay;
	char **fonts;

	// Open display
	display = XOpenDisplay(NULL);
	if (!display)
	{
		ft_printf("Cannot open display\n");
		return;
	}

	screen = DefaultScreen(display);
	root = RootWindow(display, screen);
	
	// Center window on screen
	screen_width = DisplayWidth(display, screen);
	screen_height = DisplayHeight(display, screen);
	x = (screen_width - WINDOW_WIDTH) / 2;
	y = (screen_height - WINDOW_HEIGHT) / 2;
	
	// Create window
	window = XCreateSimpleWindow(display, root, x, y, WINDOW_WIDTH, WINDOW_HEIGHT,
		0, BlackPixel(display, screen), BlackPixel(display, screen));
	
	XStoreName(display, window, "Connect 4");
	XSelectInput(display, window, ExposureMask | ButtonPressMask | StructureNotifyMask);
	
	// Set up window close handling
	wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, window, &wmDeleteWindow, 1);
	
	XMapWindow(display, window);
	
	// Set up Cairo
	surface = cairo_xlib_surface_create(display, window,
		DefaultVisual(display, screen), WINDOW_WIDTH, WINDOW_HEIGHT);
	cr = cairo_create(surface);
	
	// Set up Clay
	Clay_Cairo_Initialize(cr);
	memory = malloc(Clay_MinMemorySize());
	if (!memory)
	{
		ft_printf("Failed to allocate Clay memory\n");
		cairo_destroy(cr);
		cairo_surface_destroy(surface);
		XCloseDisplay(display);
		return;
	}
	
	arena = Clay_CreateArenaWithCapacityAndMemory(Clay_MinMemorySize(), memory);
	Clay_Initialize(arena, (Clay_Dimensions){WINDOW_WIDTH, WINDOW_HEIGHT},
		(Clay_ErrorHandler){HandleClayErrors, NULL});
	
	fonts = (char *[]){"Sans", "Sans"};
	Clay_SetMeasureTextFunction(Clay_Cairo_MeasureText, fonts);
	
	running = true;
	needsRedraw = true;
	frameDelay = (struct timespec){0, 16666667}; // 60 FPS
	
	while (running)
	{
		while (XPending(display))
		{
			XEvent event;
			XNextEvent(display, &event);
			
			switch (event.type)
			{
				case Expose:
					needsRedraw = true;
					break;
					
				case ButtonPress:
					if (event.xbutton.button == Button1)
					{
						ft_printf("Left click received\n");
						if (check_endgame(game) == 1)
						{
							ft_printf("Game is over, starting new game\n");
							// Start new game
							t_game *new_game = init_new_game(game->rows, game->cols);
							if (new_game)
							{
								free_game(game);
								game = new_game;
								ft_printf("New game started\n");
							}
						}
						else if (game->current_player == PLAYER)
						{
							ft_printf("Processing player move\n");
							int col = get_column_at_pos(event.xbutton.x, event.xbutton.y);
							ft_printf("Selected column: %d\n", col);
							
							if (insert_pawn(game, col))
							{
								ft_printf("Pawn inserted in column %d\n", col);
								needsRedraw = true;
								XFlush(display);
								
								if (!check_endgame(game))
								{
									ft_printf("Game continues, AI's turn\n");
									// Delay before AI move
									struct timespec delay = {0, 300000000}; // 0.3s
									nanosleep(&delay, NULL);
									
									ai_make_move(game);
									check_endgame(game);
								}
							}
							else
							{
								ft_printf("Failed to insert pawn\n");
							}
						}
						else
						{
							ft_printf("Not player's turn\n");
						}
						needsRedraw = true;
					}
					break;
					
				case ClientMessage:
					if ((Atom)event.xclient.data.l[0] == wmDeleteWindow)
						running = false;
					break;
			}
		}
		
		if (needsRedraw)
		{
			// Clear screen
			cairo_save(cr);
			cairo_set_source_rgb(cr, 0.1, 0.1, 0.15);
			cairo_paint(cr);
			cairo_restore(cr);
			
			// Draw UI
			createUI(game);
			Clay_RenderCommandArray commands = Clay_EndLayout();
			Clay_Cairo_Render(commands, fonts);
			
			cairo_surface_flush(surface);
			XFlush(display);
			needsRedraw = false;
		}
		
		nanosleep(&frameDelay, NULL);
	}
	
	free(memory);
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	XCloseDisplay(display);
}