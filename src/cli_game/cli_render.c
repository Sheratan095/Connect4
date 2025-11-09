#include "connect4.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

void    cli_render_board(t_game *game)
{
	static int locale_initialized = 0;
	if (!locale_initialized)
	{
		setlocale(LC_ALL, "");  // Initialize locale for proper UTF-8 handling
		locale_initialized = 1;
	}

	ft_printf("\033[3J\033[H\033[2J"); /* clear screen */

	int row, col;

	/* column headers */
	ft_printf("\n  ");
	for (col = 0; col < game->cols; col++)
	{
		char numbuf[16];
		int dlen;
		int left, right;
		const int CELL_INNER_WIDTH = 3;

		snprintf(numbuf, sizeof(numbuf), "%d", col);
		dlen = ft_strlen(numbuf);

		left = (CELL_INNER_WIDTH - dlen) / 2;
		right = CELL_INNER_WIDTH - dlen - left;

		while (left--) ft_printf(" ");
		ft_printf("%s", numbuf);
		while (right--) ft_printf(" ");
		ft_printf(" ");
	}
	ft_printf("\n");

	/* top border */
	ft_printf(" +");
	for (col = 0; col < game->cols; col++)
		ft_printf("---+");
	ft_printf("\n");

	/* rows */
	for (row = 0; row < game->rows; row++)
	{
		ft_printf(" |");
		for (col = 0; col < game->cols; col++)
		{
			if (game->board[row][col] == PLAYER)
			{
				ft_printf(PAWN_1);
				ft_printf(" |");
			}
			else if (game->board[row][col] == AI)
			{
				ft_printf(PAWN_2);
				ft_printf(" |");
			}
			else
			{
				ft_printf("   |");
			}
		}
		ft_printf("\n");

		/* row separator */
		ft_printf(" +");
		for (col = 0; col < game->cols; col++)
			ft_printf("---+");
		ft_printf("\n");
	}

	ft_printf("\n");
}
