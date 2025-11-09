#include "connect4.h"

bool is_valid_pos(t_game *game, t_pos pos)
{
	return game->board[pos.y][pos.x] == NULL_PLAYER;
}

t_moves valid_moves(t_game *game)
{
	int count = 0;

	for (int col = 0; col < game->cols; col++)
	{
		if (game->board[0][col] == NULL_PLAYER)
			count++;
	}
	t_pos *moves = malloc(sizeof(t_pos) * count);
	if (!moves)
		return (t_moves){NULL, 0};

	int index = 0;
	for (int col = 0; col < game->cols; col++)
	{
		if (game->board[0][col] == NULL_PLAYER)
		{
			moves[index].x = col;
			moves[index].y = 0;
			index++;
		}
	}
	t_moves result = {moves, count};
	return result;
}

static int64_t evaluate_window(char *window, int piece, int window_length)
{
	int64_t score = 0;
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
		if (game->board[row][col] == NULL_PLAYER)
			return row;
	}
	return -1; // Column is full
}

int64_t score_position(t_game *game)
{
	int64_t score = 0;
	int window_length = 4;

	// Score from AI's perspective
	int ai_piece = AI;
	int player_piece = PLAYER;

	// Score centre column (center control is important)
	int centre_col = game->cols / 2;
	int centre_count = 0;
	for (int row = 0; row < game->rows; row++)
	{
		if (game->board[row][centre_col] == (char)ai_piece)
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
			score += evaluate_window(window, ai_piece, window_length);
			score -= evaluate_window(window, player_piece, window_length);
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
			score += evaluate_window(window, ai_piece, window_length);
			score -= evaluate_window(window, player_piece, window_length);
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
			score += evaluate_window(window, ai_piece, window_length);
			score -= evaluate_window(window, player_piece, window_length);
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
			score += evaluate_window(window, ai_piece, window_length);
			score -= evaluate_window(window, player_piece, window_length);
		}
	}

	return score;
}

bool winning_move(t_game *game, t_player piece)
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

	// Negative diagonal (top-left to bottom-right, starting from top)
	for (int i = 3; i < game->rows; i++)
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

bool is_terminal_node(t_game *game)
{
	t_moves moves = valid_moves(game);
	bool result = winning_move(game, AI) || winning_move(game, PLAYER) || moves.count == 0;
	free(moves.positions);
	return result;
}

t_pos minmax(t_game *game, int depth, int64_t alpha, int64_t beta, bool maximizingPlayer)
{
	// Check terminal conditions first (more efficient than getting valid moves)
	if (winning_move(game, AI))
		return (t_pos){.x = -1, .y = -1, .score = 100000000 + depth}; // Prefer faster wins

	if (winning_move(game, PLAYER))
		return (t_pos){.x = -1, .y = -1, .score = -100000000 - depth}; // Avoid slower losses

	t_moves valid_loc = valid_moves(game);

	// Check for draw
	if (valid_loc.count == 0)
	{
		free(valid_loc.positions);
		return (t_pos){.x = -1, .y = -1, .score = 0};
	}

	// Depth limit reached - evaluate position
	if (depth == 0)
	{
		int64_t score = score_position(game);
		free(valid_loc.positions);
		return (t_pos){.x = -1, .y = -1, .score = score};
	}

	// Move ordering: prioritize center columns for better alpha-beta pruning
	// Center columns are more likely to lead to wins
	int center = game->cols / 2;
	for (int i = 0; i < valid_loc.count - 1; i++)
	{
		for (int j = i + 1; j < valid_loc.count; j++)
		{
			int dist_i = abs(valid_loc.positions[i].x - center);
			int dist_j = abs(valid_loc.positions[j].x - center);
			if (dist_j < dist_i)
			{
				// Swap
				t_pos temp = valid_loc.positions[i];
				valid_loc.positions[i] = valid_loc.positions[j];
				valid_loc.positions[j] = temp;
			}
		}
	}

	if (maximizingPlayer) // AI's turn
	{
		int64_t value = -999999999;
		t_pos best_move = {.x = valid_loc.positions[0].x, .y = -1, .score = -999999999};

		for (int i = 0; i < valid_loc.count; i++)
		{
			int col = valid_loc.positions[i].x;
			int row = get_next_open_row(game, col);

			t_game *temp_game = copy_game(game);
			if (temp_game == NULL)
				continue;

			// Make the move
			temp_game->current_player = AI;
			insert_pawn(temp_game, col);

			// Recursively call minmax for opponent's turn
			temp_game->current_player = PLAYER;
			t_pos new_score = minmax(temp_game, depth - 1, alpha, beta, false);

			free_game(temp_game);

			if (new_score.score > value)
			{
				value = new_score.score;
				best_move = (t_pos){.x = col, .y = row, .score = value};
			}

			alpha = (alpha > value) ? alpha : value;
			if (alpha >= beta)
				break; // Beta cutoff
		}
		free(valid_loc.positions);
		return best_move;
	}
	else // PLAYER's turn (minimizing)
	{
		int64_t value = 999999999;
		t_pos best_move = {.x = valid_loc.positions[0].x, .y = -1, .score = 999999999};

		for (int i = 0; i < valid_loc.count; i++)
		{
			int col = valid_loc.positions[i].x;
			int row = get_next_open_row(game, col);

			t_game *temp_game = copy_game(game);
			if (temp_game == NULL)
				continue;

			// Make the move
			temp_game->current_player = PLAYER;
			insert_pawn(temp_game, col);

			// Recursively call minmax for AI's turn
			temp_game->current_player = AI;
			t_pos new_score = minmax(temp_game, depth - 1, alpha, beta, true);

			free_game(temp_game);

			if (new_score.score < value)
			{
				value = new_score.score;
				best_move = (t_pos){.x = col, .y = row, .score = value};
			}

			beta = (beta < value) ? beta : value;
			if (alpha >= beta)
				break; // Alpha cutoff
		}
		free(valid_loc.positions);
		return best_move;
	}
}

bool is_empty(t_game *game)
{
	// Check if any position on the board has a piece
	for (int row = 0; row < game->rows; row++)
	{
		for (int col = 0; col < game->cols; col++)
		{
			if (game->board[row][col] != NULL_PLAYER)
				return false;
		}
	}
	return true;
}

void ai_make_move(t_game *game)
{
	// Check if board is empty (first move)
	bool board_empty = is_empty(game);

	if (board_empty)
	{
		int mid_col = game->cols / 2;
		insert_pawn(game, mid_col);
		return;
	}

	// Call minmax with AI as maximizing player (true)
	t_pos best_move = minmax(game, DEPTH, -999999999, 999999999, true);

	if (insert_pawn(game, best_move.x) == -1)
		ft_printf("AI attempted an invalid move in column %d\n", best_move.x);
}