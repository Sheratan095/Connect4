#include "connect4.h"
#include "ui_window.h"
#include "ui_loop.h"

Clay_Color getPlayerColor(char player)
{
	switch ((int)player)
	{
	case PLAYER:															 // PLAYER = 1
		return (Clay_Color){220, 0, 0, 255};		 // Bright Red
	case AI:																	 // AI = 2
		return (Clay_Color){255, 215, 0, 255};	 // Gold
	default:																	 // NULL_PLAYER = 0
		return (Clay_Color){230, 230, 230, 255}; // Light Grey
	}
}

int get_column_at_pos(int x, int y, t_game *game, t_window_context *ctx)
{
	// Calculate board position based on actual window dimensions
	int boardWidth = game->cols * CELL_SIZE;
	int boardHeight = game->rows * CELL_SIZE;
	int boardX = (ctx->width - boardWidth) / 2;
	int boardY = (ctx->height - boardHeight) / 2 + 50; // Account for title

	// First check if click is within board bounds
	if (x < boardX || x >= boardX + boardWidth || y < boardY || y >= boardY + boardHeight)
	{
		ft_printf("Click outside board area\n");
		return (-1);
	}

	// Calculate column
	int col = (x - boardX) / CELL_SIZE;

	// Validate column bounds (extra safety check)
	if (col < 0 || col >= game->cols)
	{
		ft_printf("Invalid column calculated: %d\n", col);
		return (-1);
	}

	ft_printf("Click at x=%d, y=%d (boardX=%d, boardY=%d), column=%d\n", x, y, boardX, boardY, col);
	return (col);
}

void createUI(t_game *game)
{
	const char *status;
	const char *instr;
	Clay_Color statusColor;
	Clay_String statusString;
	Clay_String instrString;

	Clay_BeginLayout();

	ft_print_char_matrix((const char **)game->board);

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
		statusString.chars = (char *)status;

		CLAY_TEXT(statusString, CLAY_TEXT_CONFIG({.fontSize = 24, .textColor = statusColor}));

		// Game board
		CLAY_AUTO_ID({.layout = {
											.sizing = {CLAY_SIZING_FIXED(game->cols * CELL_SIZE), CLAY_SIZING_FIXED(game->rows * CELL_SIZE)},
											.layoutDirection = CLAY_TOP_TO_BOTTOM},
									.backgroundColor = {40, 80, 160, 255},
									.cornerRadius = CLAY_CORNER_RADIUS(12)})
		{
			for (int row = 0; row < game->rows; row++)
			{
				CLAY_AUTO_ID({.layout = {
													.sizing = {CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(CELL_SIZE)},
													.layoutDirection = CLAY_LEFT_TO_RIGHT}})
				{
					for (int col = 0; col < game->cols; col++)
					{
						// Get cell value directly from board and pass to getPlayerColor
						int cellValue = game->board[row][col];
						Clay_Color cellColor = getPlayerColor(cellValue);
						bool isLast = (row == game->last_row && col == game->last_col);

						// Combine padding and cell into single element to reduce Clay element count
						CLAY_AUTO_ID({.layout = {
															.sizing = {CLAY_SIZING_FIXED(CELL_SIZE), CLAY_SIZING_FIXED(CELL_SIZE)},
															.padding = {6, 6, 6, 6},
															.childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER}},
													.backgroundColor = cellColor,
													.cornerRadius = CLAY_CORNER_RADIUS(CELL_SIZE / 2 - 8),
													.border = {.color = {255, 255, 255, 255}, .width = {.left = isLast ? 3 : 0, .right = isLast ? 3 : 0, .top = isLast ? 3 : 0, .bottom = isLast ? 3 : 0}}}) {}
					}
				}
			}
		}

		// Instructions
		instr = game->game_over ? "Click anywhere to restart" : "Click a column to place your piece";
		instrString.isStaticallyAllocated = false;
		instrString.length = strlen(instr);
		instrString.chars = (char *)instr;

		CLAY_TEXT(instrString, CLAY_TEXT_CONFIG({.fontSize = 18,
																						 .textColor = {180, 180, 180, 255}}));
	}
	// Don't call Clay_EndLayout() here - let the caller handle it
}

void HandleClayErrors(Clay_ErrorData errorData)
{
	ft_printf("==== CLAY ERROR ====\n");
	printf("Error: %.*s\n", (int)errorData.errorText.length, errorData.errorText.chars);
	ft_printf("This usually means Clay ran out of internal memory.\n");
	ft_printf("Try reducing board size or increasing memory allocation.\n");
	ft_printf("====================\n");
}

void start_game_ui(t_game *game)
{
	t_window_context *ctx = init_window(game);
	if (!ctx)
		return;

	run_game_loop(ctx, game);
	cleanup_window(ctx);
}