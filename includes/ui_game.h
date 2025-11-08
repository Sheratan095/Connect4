#ifndef UI_GAME_H
#define UI_GAME_H

#include "connect4.h"

// Functions shared between UI components
Clay_Color getPlayerColor(char player);
int get_column_at_pos(int x, int y);
void createUI(t_game *game);
void HandleClayErrors(Clay_ErrorData errorData);

#endif