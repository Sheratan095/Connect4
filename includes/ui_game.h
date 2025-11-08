#ifndef UI_GAME_H
#define UI_GAME_H

#include "connect4.h"

// Forward declaration
typedef struct s_window_context t_window_context;

// Functions shared between UI components
Clay_Color getPlayerColor(char player);
int get_column_at_pos(int x, int y, t_game *game, t_window_context *ctx);
void createUI(t_game *game);
void HandleClayErrors(Clay_ErrorData errorData);

#endif