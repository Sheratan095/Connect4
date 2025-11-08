#include "connect4.h"

static void	switch_player(t_game *game);
static int	check_endgame(t_game *game);

void	start_game(t_game *game)
{
	cli_render_board(game);

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

// Change the current player
static void	switch_player(t_game *game)
{
	if (game->current_player == PLAYER)
		game->current_player = AI;
	else
		game->current_player = PLAYER;
}

static int	check_endgame(t_game *game)
{
	t_game_result	result = get_winner(game);

	if (result == PLAYER_WINS)
	{
		ft_printf("Player %s wins!\n", PAWN_1);
		return (1);
	}
	if (result == AI_WINS)
	{
		ft_printf("Player %s wins!\n", PAWN_2);
		return (1);
	}
	if (result == DRAW)
	{
		ft_printf("The game is a draw!\n");
		return (1);
	}

	return (0);
}