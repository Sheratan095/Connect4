#ifndef CONNECT4_H
#define CONNECT4_H

#include "corekit.h"

#ifndef PAWN_1
	#define PAWN_1 "🔴"
#endif

#ifndef PAWN_2
	#define PAWN_2 "🟡"
#endif

#define MIN_ROWS 6
#define MIN_COLS 7

#ifndef MAX_ROWS
	#define MAX_ROWS 20
#endif

#ifndef MAX_COLS
	#define MAX_COLS 20
#endif

#define EMPTY_BOARD_CHAR '.'

typedef enum	e_player
{
	NULL_PLAYER = 0,
	PLAYER = 1,
	AI = 2,
}	t_player;

typedef enum	e_game_result
{
	NO_WINNER = 0,
	PLAYER_WINS = 1,
	AI_WINS = 2,
	DRAW = 3,
}	t_game_result;

typedef struct	s_game
{
	char		**board;
	t_player	current_player;
	int			rows;
	int			cols;
	t_bool		gui_mode;
}	t_game;

void			cli_render_board(t_game *game);

void			start_game(t_game *game);

void			ai_make_move(t_game *game);

int				insert_pawn(t_game *game, int column);

int				player_make_move(t_game *game);

//--------------------HELPER FUNCTIONS--------------------//

t_game			*init_new_game(int rows, int cols, t_bool gui_mode);

void			free_game(t_game *game);

t_game_result	get_winner(t_game *game);


#endif