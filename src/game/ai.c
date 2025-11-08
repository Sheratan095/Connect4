#include "connect4.h"

t_bool is_valid_pos(t_game *game, t_pos pos)
{
	return game->board[pos.y][pos.x] == EMPTY_BOARD_CHAR;
}

t_moves valid_moves(t_game *game)
{
	int count = 0;

	for (int col = 0; col < game->cols; col++)
	{
		if (game->board[0][col] == EMPTY_BOARD_CHAR)
			count++;
	}
	t_pos *moves = malloc(sizeof(t_pos) * count);
	if (!moves)
		return (t_moves){NULL, 0};

	int index = 0;
	for (int col = 0; col < game->cols; col++)
	{
		if (game->board[0][col] == EMPTY_BOARD_CHAR)
		{
			moves[index].x = col;
			moves[index].y = 0;
			index++;
		}
	}
	t_moves result = {moves, count};
	return result;
}

static u_int64_t evaluate_window(char *window, int piece, int window_length)
{
	u_int64_t score = 0;
	int piece_count = 0;
	int opponent_count = 0;
	int opponent_piece = (piece == PLAYER) ? AI : PLAYER;

	for (int i = 0; i < window_length; i++)
	{
		if (window[i] == (char)piece)
			piece_count++;
		else if (window[i] == (char)opponent_piece)
			opponent_count++;
	}

	if (opponent_count == 0)
	{
		if (piece_count == 4)
			score += 500; // Four in a row (winning move)
		else if (piece_count == 3)
			score += 50; // Three in a row
		else if (piece_count == 2)
			score += 10; // Two in a row
		else if (piece_count == 1)
			score += 1; // One piece
	}
	else if (piece_count == 0 && opponent_count < window_length)
		score -= 5; // Opponent could potentially build here

	return score;
}

int get_next_open_row(t_game *game, int col)
{
	for (int row = game->rows - 1; row >= 0; row--)
	{
		if (game->board[row][col] == EMPTY_BOARD_CHAR)
			return row;
	}
	return -1; // Column is full
}

u_int64_t score_position(t_game *game)
{
	u_int64_t score = 0;
	int window_length = 4;
	int piece = game->current_player;

	// Score centre column
	int centre_col = game->cols / 2;
	int centre_count = 0;
	for (int row = 0; row < game->rows; row++)
	{
		if (game->board[row][centre_col] == piece)
			centre_count++;
	}
	score += centre_count * 3;

	// Score horizontal positions
	for (int r = 0; r < game->rows; r++)
	{
		for (int c = 0; c <= game->cols - window_length; c++)
		{
			char window[window_length];
			for (int i = 0; i < window_length; i++)
				window[i] = game->board[r][c + i];
			score += evaluate_window(window, piece, window_length);
		}
	}

	// Score vertical positions
	for (int c = 0; c < game->cols; c++)
	{
		for (int r = 0; r <= game->rows - window_length; r++)
		{
			char window[window_length];
			for (int i = 0; i < window_length; i++)
				window[i] = game->board[r + i][c];
			score += evaluate_window(window, piece, window_length);
		}
	}

	// Score positive diagonals (bottom-left to top-right)
	for (int r = 0; r <= game->rows - window_length; r++)
	{
		for (int c = 0; c <= game->cols - window_length; c++)
		{
			char window[window_length];
			for (int i = 0; i < window_length; i++)
				window[i] = game->board[r + i][c + i];
			score += evaluate_window(window, piece, window_length);
		}
	}

	// Score negative diagonals (top-left to bottom-right)
	for (int r = window_length - 1; r < game->rows; r++)
	{
		for (int c = 0; c <= game->cols - window_length; c++)
		{
			char window[window_length];
			for (int i = 0; i < window_length; i++)
				window[i] = game->board[r - i][c + i];
			score += evaluate_window(window, piece, window_length);
		}
	}

	return score;
}

t_bool winning_move(t_game *game, t_player piece)
{
	for (int i = 0; i < game->rows; i++)
	{
		for (int j = 0; j < game->cols - 3; j++)
		{
			if (game->board[i][j] == (char)piece &&
					game->board[i][j + 1] == (char)piece &&
					game->board[i][j + 2] == (char)piece &&
					game->board[i][j + 3] == (char)piece)
				return true;
		}
	}
	for (int i = 0; i < game->rows - 3; i++)
	{
		for (int j = 0; j < game->cols; j++)
		{
			if (game->board[i][j] == (char)piece &&
					game->board[i + 1][j] == (char)piece &&
					game->board[i + 2][j] == (char)piece &&
					game->board[i + 3][j] == (char)piece)
				return true;
		}
	}
	for (int i = 0; i < game->rows - 3; i++)
	{
		for (int j = 0; j < game->cols - 3; j++)
		{
			if (game->board[i][j] == (char)piece &&
					game->board[i + 1][j + 1] == (char)piece &&
					game->board[i + 2][j + 2] == (char)piece &&
					game->board[i + 3][j + 3] == (char)piece)
				return true;
		}
	}

	for (int i = 0; i < game->rows - 3; i++)
	{
		for (int j = 0; j < game->cols - 3; j++)
		{
			if (game->board[i][j] == (char)piece &&
					game->board[i - 1][j + 1] == (char)piece &&
					game->board[i - 2][j + 2] == (char)piece &&
					game->board[i - 3][j + 3] == (char)piece)
				return true;
		}
	}
	return false;
}

t_bool is_terminal_node(t_game *game)
{
	return winning_move(game, AI) || winning_move(game, PLAYER) || valid_moves(game).count == 0;
}

t_pos minmax(t_game *game, int depth, u_int64_t alpha, u_int64_t beta, t_bool maximizingPlayer)
{
	t_moves valid_loc = valid_moves(game);

	t_bool is_terminal = is_terminal_node(game);
	if (depth == 0 || is_terminal)
	{
		if (is_terminal)
		{
			if (winning_move(game, AI))
			{
				return (t_pos){-1, -1, 99999999};
			}
			else if (winning_move(game, PLAYER))
			{
				return (t_pos){-1, -1, -99999999};
			}
			else
			{
				return (t_pos){-1, -1, 0};
			}
		}
		else
		{
			u_int64_t score = score_position(game);
			return (t_pos){-1, -1, score};
		}
	}

	if (maximizingPlayer)
	{
		u_int64_t value = 0;
		t_pos best_move = {-1, -1, -99999999};

		for (int i = 0; i < valid_loc.count; i++)
		{
			int row = get_next_open_row(game, valid_loc.positions[i].x);

			t_game *temp_game = copy_game(game);
			insert_pawn(temp_game, valid_loc.positions[i].x);
			t_pos new_score = minmax(temp_game, depth - 1, alpha, beta, false);
			if (new_score.score > value)
			{
				value = new_score.score;
				best_move = (t_pos){valid_loc.positions[i].x, row, value};
			}
			free_game(temp_game);
			alpha = (alpha > value) ? alpha : value;
			if (alpha >= beta)
				break;
		}
		return best_move;
	}
	else
	{
		u_int64_t value = 99999999;
		t_pos best_move = {-1, -1, 99999999};

		for (int i = 0; i < valid_loc.count; i++)
		{
			int row = get_next_open_row(game, valid_loc.positions[i].x);

			t_game *temp_game = copy_game(game);
			if (temp_game == NULL)
				continue;
			insert_pawn(temp_game, valid_loc.positions[i].x);
			t_pos new_score = minmax(temp_game, depth - 1, alpha, beta, true);
			if (new_score.score < value)
			{
				value = new_score.score;
				best_move = (t_pos){valid_loc.positions[i].x, row, value};
			}
			free_game(temp_game);
			beta = (beta < value) ? beta : value;
			if (alpha >= beta)
				break;
		}
		return best_move;
	}
}

t_bool is_empty(char **board)
{
	for (int col = 0; col < 7; col++)
	{
		if (board[5][col] != EMPTY_BOARD_CHAR)
			return false;
	}
	return true;
}

void ai_make_move(t_game *game)
{
	// Check if board is empty (first move) - check if center column is empty
	t_bool board_empty = is_empty(game->board);

	if (board_empty)
	{
		int mid_col = game->cols / 2;
		insert_pawn(game, mid_col);
		return;
	}
	ft_printf("AI is thinking...\n");

	t_pos best_move = minmax(game, 5, -99999999, 99999999, true);
	ft_printf("AI chooses column %d\n", best_move.x);
	if (insert_pawn(game, best_move.x) == -1)
		ft_printf("AI attempted an invalid move in column %d\n", best_move.x);
}