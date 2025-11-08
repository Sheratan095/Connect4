#include "connect4.h"

// TO DO remove
int			player_make_move(t_game *game);

void	ai_make_move(t_game *game)
{
	if (player_make_move(game) == -1)
	{
		ft_printf("Game exited by player.\n");
		return ;
	}
}