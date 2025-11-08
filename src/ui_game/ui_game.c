#include "connect4.h"

// TO DO remove
int	player_make_move(t_game *game);

void	start_game_ui(t_game *game)
{
	cli_render_board(game);
	ft_printf("GUI mode is not yet implemented.\n");

	while (true)
	{
		if (game->current_player == AI)
			ai_make_move(game);

		if (game->current_player == PLAYER)
		{
			if (player_make_move(game) == -1)
			{
				ft_printf("Game exited by player.\n");
				break ;
			}
		}

		cli_render_board(game);

		if (check_endgame(game))
			break ;

		switch_player(game);
	}
}
