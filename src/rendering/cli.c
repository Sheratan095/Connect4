#define _XOPEN_SOURCE 700
#include "connect4.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>

/*
 * Cells look like: | ⬤ | and rows/columns are separated by +---+ lines.
*/
void	cli_render_board(t_game *game)
{
	ft_printf("\033[3J\033[H\033[2J");
	int	row;
	int	col;

	/* Column headers */
	ft_printf("\n  ");
	for (col = 0; col < game->cols; col++)
	{
		char numbuf[16];
		int i, dlen;
		snprintf(numbuf, sizeof(numbuf), "%d", col);
		dlen = (int)strlen(numbuf);
		/* center the number inside a CELL_INNER_WIDTH-wide cell */
		const int CELL_INNER_WIDTH = 3;
		int left = (CELL_INNER_WIDTH - dlen) / 2;
		int right = CELL_INNER_WIDTH - dlen - left;
		for (i = 0; i < left; i++) ft_printf(" ");
		ft_printf("%s", numbuf);
		for (i = 0; i < right; i++) ft_printf(" ");
		/* spacer between headers so it lines up with border */
		ft_printf(" ");
	}
	ft_printf("\n");

	/* Top border */
	ft_printf(" +");
	for (col = 0; col < game->cols; col++)
		ft_printf("---+");
	ft_printf("\n");

	/* helper: compute display width of a UTF-8 string in terminal columns */
	{
		/* keep locale init local and idempotent */
		static int locale_inited = 0;
		if (!locale_inited)
		{
			setlocale(LC_CTYPE, "");
			locale_inited = 1;
		}
	}

	/* Rows */
	for (row = 0; row < game->rows; row++)
	{
		int i;
		ft_printf(" |");
		for (col = 0; col < game->cols; col++)
		{
			const char *cell = NULL;
			if (game->board[row][col] == PLAYER)
				cell = PAWN_1;
			else if (game->board[row][col] == AI)
				cell = PAWN_2;

			const int CELL_INNER_WIDTH = 3;

			if (!cell || cell[0] == '\0')
			{
				/* empty cell: three spaces */
				for (i = 0; i < CELL_INNER_WIDTH; i++) ft_printf(" ");
				ft_printf("|");
				continue;
			}

			/* convert UTF-8 to wchar_t buffer to measure display width */
			wchar_t wbuf[32];
			mbstate_t ps;
			size_t converted;
			const char *src = cell;
			memset(&ps, 0, sizeof(ps));
			converted = mbsrtowcs(wbuf, &src, sizeof(wbuf) / sizeof(wchar_t), &ps);
			int dispw = -1;
			if (converted != (size_t)-1)
			{
				dispw = (int)wcswidth(wbuf, converted);
			}
			if (dispw < 0)
			{
				/* fallback to byte-length heuristic */
				dispw = (int)strlen(cell);
			}

			if (dispw >= CELL_INNER_WIDTH)
			{
				/* content too wide, fall back to left-aligned with single space */
				ft_printf(" ");
				ft_printf("%s", cell);
				/* pad remaining if any */
				for (i = 1 + (int)strlen(cell); i < CELL_INNER_WIDTH; i++) ft_printf(" ");
				ft_printf("|");
			}
			else
			{
				int left = (CELL_INNER_WIDTH - dispw) / 2;
				int right = CELL_INNER_WIDTH - dispw - left;
				for (i = 0; i < left; i++) ft_printf(" ");
				ft_printf("%s", cell);
				for (i = 0; i < right; i++) ft_printf(" ");
				ft_printf("|");
			}
		}
		ft_printf("\n");

		/* Separator between rows */
		ft_printf(" +");
		for (col = 0; col < game->cols; col++)
			ft_printf("---+");
		ft_printf("\n");
	}

	ft_printf("\n");
}
