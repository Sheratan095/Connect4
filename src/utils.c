#include "connect4.h"

t_game	*init_new_game(int rows, int cols)
{
	t_game	*game = malloc(sizeof(t_game));
	if (!game)
		return (NULL);

	game->rows = rows;
	game->cols = cols;

	game->current_player = PLAYER;

	game->board = malloc(sizeof(char *) * 6);
	for (int i = 0; i < 6; i++)
	{
		game->board[i] = malloc(sizeof(char) * 7);
		ft_bzero(game->board[i], sizeof(char) * 7);
	}

	return (game);
}

void	free_game(t_game *game)
{
	if (!game)
		return ;

	ft_free_matrix((void **)game->board);
	free(game);
}

int	insert_pawn(t_game *game, int column)
{
	for (int row = game->rows - 1; row >= 0; row--)
	{
		if (game->board[row][column] == NULL_PLAYER)
		{
			game->board[row][column] = game->current_player;
			return (row);
		}
	}

	return (-1);
}

// Get the winner of the game
t_game_result	get_winner(t_game *game)
{
	// Check horizontal
	for (int row = 0; row < game->rows; row++)
	{
		for (int col = 0; col < game->cols - 3; col++)
		{
			int	p = game->board[row][col];
			if (p != NULL_PLAYER &&
				p == game->board[row][col + 1] &&
				p == game->board[row][col + 2] &&
				p == game->board[row][col + 3])
			{
				return (p == PLAYER) ? PLAYER_WINS : AI_WINS;
			}
		}
	}

	// Check vertical
	for (int col = 0; col < game->cols; col++)
	{
		for (int row = 0; row < game->rows - 3; row++)
		{
			int	p = game->board[row][col];
			if (p != NULL_PLAYER &&
				p == game->board[row + 1][col] &&
				p == game->board[row + 2][col] &&
				p == game->board[row + 3][col])
			{
				return (p == PLAYER) ? PLAYER_WINS : AI_WINS;
			}
		}
	}

	// TO DO 3 is magic number here

	// Check diagonal (bottom-left to top-right)
	for (int row = 3; row < game->rows; row++)
	{
		for (int col = 0; col < game->cols - 3; col++)
		{
			int	p = game->board[row][col];
			if (p != NULL_PLAYER &&
				p == game->board[row - 1][col + 1] &&
				p == game->board[row - 2][col + 2] &&
				p == game->board[row - 3][col + 3])
			{
				return (p == PLAYER) ? PLAYER_WINS : AI_WINS;
			}
		}
	}

	// Check diagonal (top-left to bottom-right)
	for (int row = 0; row < game->rows - 3; row++)
	{
		for (int col = 0; col < game->cols - 3; col++)
		{
			int	p = game->board[row][col];
			if (p != NULL_PLAYER &&
				p == game->board[row + 1][col + 1] &&
				p == game->board[row + 2][col + 2] &&
				p == game->board[row + 3][col + 3])
			{
				return (p == PLAYER) ? PLAYER_WINS : AI_WINS;
			}
		}
	}

	return (NO_WINNER);
}