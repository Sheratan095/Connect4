#include "connect4.h"

static bool valid_input(int argc, char **argv, bool *gui);

int main(int argc, char **argv)
{
	bool gui;

	if (!valid_input(argc, argv, &gui))
		return (1);

	int rows = ft_atoi(argv[1]);
	int cols = ft_atoi(argv[2]);

	t_game *new_game = init_new_game(rows, cols);
	if (!new_game)
	{
		ft_printf("Error: Could not start a new game.\n");
		return (1);
	}

	if (gui)
		start_game_ui(new_game);
	else
		start_game_cli(new_game);

	free_game(new_game);

	return (0);
}

static bool valid_input(int argc, char **argv, bool *gui)
{
	if (MAX_COLS < MIN_COLS || MAX_ROWS < MIN_ROWS)
	{
		ft_printf("Error: MAX_ROWS and MAX_COLS must be greater than or equal to MIN_LINES and MIN_COLS respectively.\n");
		return (false);
	}

	if (argc != 3)
	{
		if (argc == 4)
		{
			if (ft_strcmp(argv[3], "--gui") != 0)
			{
				ft_printf("Error: Invalid argument '%s'. Expected '--gui'.\n", argv[3]);
				return (false);
			}
			*gui = true;
		}
		else
		{
			ft_printf("Usage: %s <rows> <columns>\n", argv[0]);
			return (false);
		}
	}

	if (!ft_is_string_numeric(argv[1]) || !ft_is_string_numeric(argv[2]))
	{
		ft_printf("Error: Rows and columns must be numeric values.\n");
		return (false);
	}

	int rows = ft_atoi(argv[1]);
	int cols = ft_atoi(argv[2]);

	if (rows < MIN_ROWS || rows > MAX_ROWS)
	{
		ft_printf("Error: Rows must be between %d and %d.\n", MIN_ROWS, MAX_ROWS);
		return (false);
	}

	if (cols < MIN_COLS || cols > MAX_COLS)
	{
		ft_printf("Error: Columns must be between %d and %d.\n", MIN_COLS, MAX_COLS);
		return (false);
	}

	return (true);
}