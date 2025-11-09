#ifndef CONNECT4_H
#define CONNECT4_H

#include "corekit.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define _POSIX_C_SOURCE 200809L
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#define BOARD_ROWS 6
#define BOARD_COLS 7
#define CELL_SIZE 70
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 650
#include "clay.h"
#include "clay_render.h"

#ifndef PAWN_1
#define PAWN_1 " 🔴"
#endif

#ifndef PAWN_2
#define PAWN_2 " 🟡"
#endif

#define MIN_ROWS 6
#define MIN_COLS 7

#ifndef MAX_ROWS
#define MAX_ROWS 20
#endif

#ifndef MAX_COLS
#define MAX_COLS 20
#endif

#ifndef DEPTH
#define DEPTH 1
#endif

typedef enum e_player
{
	NULL_PLAYER = 0,
	PLAYER = 1,
	AI = 2,
} t_player;

typedef enum e_game_result
{
	NO_WINNER = 0,
	PLAYER_WINS = 1,
	AI_WINS = 2,
	DRAW = 3,
} t_game_result;

typedef struct s_pos
{
	int x;
	int y;
	int64_t score;
} t_pos;

typedef struct s_moves
{
	t_pos *positions;
	int count;
} t_moves;

typedef struct s_game
{
	char **board;
	t_player current_player;
	int rows;
	int cols;
	bool game_over;
	t_player winner;
	int last_row;
	;
	int last_col;
} t_game;

void start_game_cli(t_game *game);

void start_game_ui(t_game *game);

void ai_make_move(t_game *game);

int insert_pawn(t_game *game, int column);

//--------------------HELPER FUNCTIONS--------------------//

t_game *init_new_game(int rows, int cols);

void free_game(t_game *game);

t_game *copy_game(t_game *original);

t_game_result get_winner(t_game *game);

void switch_player(t_game *game);

int check_endgame(t_game *game);

void cli_render_board(t_game *game);

#endif