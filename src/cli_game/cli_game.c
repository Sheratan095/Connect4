#include "connect4.h"

int player_make_move(t_game *game);

void start_game_cli(t_game *game)
{
	cli_render_board(game);

	while (true)
	{
		if (game->current_player == AI)
		{
			ft_printf("AI is making a move.\n");
			ai_make_move(game);
		}

		if (game->current_player == PLAYER)
		{
			ft_printf("Player is making a move.\n");
			if (player_make_move(game) == -1)
			{
				ft_printf("Game exited by player.\n");
				break;
			}
		}

		cli_render_board(game);

		if (check_endgame(game))
			break;

		switch_player(game);
	}
}
