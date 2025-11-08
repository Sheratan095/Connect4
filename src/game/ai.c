#include "connect4.h"

void	ai_make_move(t_game *game)
{
	if (player_make_move(game) == -1)
	{
		ft_printf("Game exited by player.\n");
		return ;
	}
}