#include "connect4.h"

static void	switch_player(t_game *game);
static int	check_endgame(t_game *game);

void	start_new_game(int rows, int cols)
{
	t_game	*new_game = init_new_game(rows, cols);

	if (!new_game)
	{
		ft_printf("Error: Could not start a new game.\n");
		return ;
	}

	cli_render_board(new_game);

	while (true)
	{
		if (new_game->current_player == AI)
			ai_make_move(new_game);

		if (new_game->current_player == PLAYER)
		{
			if (player_make_move(new_game) == -1)
			{
				ft_printf("Game exited by player.\n");
				break ;
			}
		}

		cli_render_board(new_game);

		if (check_endgame(new_game))
			break ;

		switch_player(new_game);
	}

	free_game(new_game);
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