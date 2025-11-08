#include "connect4.h"

static int	get_move(t_game *game);

int	player_make_move(t_game *game)
{
	int	column_choice = get_move(game);
	if (column_choice == -1)
		return (-1);

	while (insert_pawn(game, column_choice) == -1)
	{
		ft_printf("Column %d is full. Try another column.\n", column_choice);
		column_choice = get_move(game);
		if (column_choice == -1)
			return (-1);
	}
	
	return (0);
}

static int	get_move(t_game *game)
{
	int	column = -1;
	int	game_over = false;
	int	valid_move = false;

	while (!valid_move && !game_over)
	{
		ft_printf("Player %s, enter your move (0-6): ", 
			(game->current_player == PLAYER) ? PAWN_1 : PAWN_2);

		char	*input = get_next_line(0, false);
		if (!input)
		{
			ft_printf("Error reading input. Please try again.\n");
			continue;
		}

		if (ft_strcmp(input, "exit\n") == 0 || ft_strcmp(input, "q\n") == 0)
			game_over = true;
		else
		{
			column = ft_atoi(input);
			if (column < 0 || column >= game->cols)
				ft_printf("Invalid column. Please enter a number between 0 and %d.\n", game->cols - 1);
			else
				valid_move = true;
		}

		free(input);
	}

	get_next_line(0, true); // Clean up static memory in get_next_line
	return (column);
}