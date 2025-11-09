#include "connect4.h"
#include "ui_window.h"
#include "ui_game.h"

static void redraw(t_window_context *ctx, t_game *game);

static void handle_button_press(t_window_context *ctx, t_game *game, XButtonEvent *event)
{
	if (event->button != Button1)
		return;

	if (game->game_over)
	{
		// Reset game state
		for (int row = 0; row < game->rows; row++)
		{
			for (int col = 0; col < game->cols; col++)
			{
				game->board[row][col] = NULL_PLAYER;
			}
		}
		game->game_over = false;
		game->winner = NULL_PLAYER;
		game->current_player = PLAYER;
		game->last_row = -1;
		game->last_col = -1;
		return;
	}

	if (game->current_player == PLAYER)
	{
		int col = get_column_at_pos(event->x, event->y, game, ctx);

		if (col >= 0) // Only try to insert if column is valid
		{
			int row = insert_pawn(game, col);
			if (row != -1)
			{
				// Check for win condition after player move
				if (check_endgame(game))
				{
					game->game_over = true;
					game->winner = PLAYER;
				}
				else
					switch_player(game); // Only switch if move was successful and game not over

				XFlush(ctx->display);
				redraw(ctx, game);
			}
		}
		// If col < 0, click was outside board - already logged by get_column_at_pos
	}

	if (game->current_player == AI && !game->game_over)
	{
		ai_make_move(game);

		// Check for win condition after AI move
		if (check_endgame(game))
		{
			game->game_over = true;
			game->winner = AI;
		}
		else
		{
			switch_player(game);
		}
		XFlush(ctx->display);
		redraw(ctx, game);
	}
}

static void redraw(t_window_context *ctx, t_game *game)
{
	// Clear screen
	cairo_save(ctx->cr);
	cairo_set_source_rgb(ctx->cr, 0.1, 0.1, 0.15);
	cairo_paint(ctx->cr);
	cairo_restore(ctx->cr);

	// Draw UI
	createUI(game);
	Clay_RenderCommandArray commands = Clay_EndLayout();
	Clay_Cairo_Render(commands, ctx->fonts);

	cairo_surface_flush(ctx->surface);
	XFlush(ctx->display);
}

void run_game_loop(t_window_context *ctx, t_game *game)
{
	bool running = true;
	bool needsRedraw = true;										// Start with true to draw initial frame
	struct timespec frameDelay = {0, 16666667}; // 60 FPS

	// Check if AI should make the first move
	if (game->current_player == AI && !game->game_over)
	{
		ft_printf("AI is the first to play, making move\n");
		ai_make_move(game);
		switch_player(game);
		redraw(ctx, game);
	}

	while (running)
	{
		while (XPending(ctx->display))
		{
			XEvent event;
			memset(&event, 0, sizeof(XEvent)); // Initialize to zero for MemorySanitizer
			XNextEvent(ctx->display, &event);

			switch (event.type)
			{
				case Expose:
					needsRedraw = true;
					break;

				case ButtonPress:
					handle_button_press(ctx, game, &event.xbutton);
					needsRedraw = true;
					break;

				// Handle window close event
				case ClientMessage:
					if ((Atom)event.xclient.data.l[0] == ctx->wmDeleteWindow)
						running = false;
					break;
			}
		}

		// Only redraw when needed
		if (needsRedraw)
		{
			redraw(ctx, game);
			needsRedraw = false;
		}

		nanosleep(&frameDelay, NULL);
	}
}
