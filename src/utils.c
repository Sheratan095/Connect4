#include "connect4.h"

t_game *init_new_game(int rows, int cols)
{
	t_game *game = malloc(sizeof(t_game));
	if (!game)
		return (NULL);

	game->rows = rows;
	game->cols = cols;
	// Random first player
	game->current_player = (rand() % 2) ? PLAYER : AI;
	game->game_over = false;
	game->winner = NULL_PLAYER;
	game->last_row = -1;
	game->last_col = -1;

	game->board = ft_calloc(rows + 1, sizeof(char *));
	if (!game->board)
	{
		free(game);
		return (NULL);
	}
	for (int i = 0; i < rows; i++)
	{
		game->board[i] = ft_calloc(cols + 1, sizeof(char));
		if (!game->board[i])
		{
			// free previously allocated rows and board
			for (int j = 0; j < i; j++)
				free(game->board[j]);
			free(game->board);
			free(game);
			return (NULL);
		}
		// Initialize row with empty cells
		for (int j = 0; j < cols; j++)
			game->board[i][j] = NULL_PLAYER;
		game->board[i][cols] = '\0'; // Null terminate the string
	}
	game->board[rows] = NULL; // Null terminate the array

	return (game);
}

t_game *copy_game(t_game *original)
{
	t_game *copy = init_new_game(original->rows, original->cols);
	if (!copy)
		return NULL;

	copy->current_player = original->current_player;

	for (int r = 0; r < original->rows; r++)
	{
		for (int c = 0; c < original->cols; c++)
		{
			copy->board[r][c] = original->board[r][c];
		}
	}

	return copy;
}

void free_game(t_game *game)
{
	if (!game)
		return;

	ft_free_matrix((void **)game->board);
	free(game);
}

int insert_pawn(t_game *game, int column)
{
	for (int row = game->rows - 1; row >= 0; row--)
	{
		if (game->board[row][column] == NULL_PLAYER) // Check for empty cell
		{
			game->board[row][column] = game->current_player; // Use player enum value directly
			game->last_row = row;							 // Update last move position
			game->last_col = column;
			return (row);
		}
	}

	return (-1);
}

// Get the winner of the game
t_game_result get_winner(t_game *game)
{
	// Check horizontal
	for (int row = 0; row < game->rows; row++)
	{
		for (int col = 0; col < game->cols - 3; col++)
		{
			int p = game->board[row][col];
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
			int p = game->board[row][col];
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
			int p = game->board[row][col];
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
			int p = game->board[row][col];
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

// Change the current player
void switch_player(t_game *game)
{
	if (game->current_player == PLAYER)
		game->current_player = AI;
	else
		game->current_player = PLAYER;
}

int check_endgame(t_game *game)
{
	t_game_result result = get_winner(game);

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