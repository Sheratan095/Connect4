NAME		= connect4

# 🔴🟠🟡🟢🔵🟣⚫⚪🟤

PAWN_1 = 🔴
PAWN_2 = 🟡

COREKIT_PATH = ./lib/corekit
# MLX_PATH = ./lib/mlx
MLX_PATH =

VALGRIND =  valgrind --leak-check=full --track-origins=yes --track-fds=yes --show-leak-kinds=all

INCLUDES = -I$(COREKIT_PATH)/includes -I./includes

SRC = src/main.c \
	src/cli_game/cli_render.c \
	src/cli_game/cli_game.c \
	src/cli_game/cli_player.c \
	src/ui_game/ui_game.c \
	src/ai.c \
	src/utils.c

FLAGS	= -g
FLAGS	+= -Wall -Werror -Wextra
FLAGS	+= -DPAWN_1=\"$(PAWN_1)\" -DPAWN_2=\"$(PAWN_2)\"
FLAGS	+= -DMAX_ROWS=19 -DMAX_COLS=20

LIBCOREKIT = $(COREKIT_PATH)/libcorekit.a

$(NAME): $(SRC) $(LIBCOREKIT)
	@echo "$(GREEN)[COREKIT]:\t COREKIT CREATED$(RESET)"
	@cc $(SRC) $(INCLUDES) $(FLAGS) -lm -L$(COREKIT_PATH) -lcorekit -o $(NAME)
	@echo "$(GREEN)[$(NAME)]:\t PROJECT COMPILED$(RESET)"

$(LIBCOREKIT):
	@${MAKE} -sC $(COREKIT_PATH)
	@echo "$(GREEN)[COREKIT]:\t COREKIT BUILT$(RESET)"

all:$(NAME)

clean:
	@if [ -d "$(COREKIT_PATH)" ]; then \
		${MAKE} -C $(COREKIT_PATH) clean -s; \
		echo "$(RED)[COREKIT]:\t COREKIT CLEAN$(RESET)"; \
	fi

fclean: clean
	@rm -f $(NAME)
	@rm -f $(BONUS_NAME)
	@if [ -d "$(COREKIT_PATH)" ]; then \
		${MAKE} -C $(COREKIT_PATH) fclean -s; \
		echo "$(RED)[COREKIT]:\t COREKIT FCLEAN$(RESET)"; \
	fi

re: download fclean corekit_pull all

#used to ensure that corekit is always up to date
corekit_pull:
	@git -C lib/corekit/ pull --quiet
	@echo "$(GREEN)[COREKIT]:\t PULLED$(RESET)";

MLX_LINK=
download:
	@if [ -d "lib" ]; then \
		echo "$(RED)[LIB]:\t\t LIB FOLDER ALREADY EXISTS$(RESET)"; \
	else \
		mkdir -p lib; \
	fi; \
	if [ ! -d "$(COREKIT_PATH)" ]; then \
		git clone -q https://github.com/Sheratan095/Corekit.git lib/corekit && \
		echo "$(GREEN)[COREKIT]:\t COREKIT DOWNLOADED$(RESET)"; \
	else \
		echo "$(RED)[COREKIT]:\t COREKIT ALREADY EXISTS$(RESET)"; \
	fi

remove_libs:
	@rm -fr $(COREKIT_PATH)
	@echo "$(RED)[COREKIT]:\t COREKIT REMOVED$(RESET)";
	@rm -fr lib/

args = 10 10 --gui

test: all
	 ./$(NAME) $(args)

val: all
	$(VALGRIND) ./$(NAME) $(args)

.PHONY: download

#COLORS

GREEN=\033[0;32m
RED=\033[0;31m
BLUE=\033[0;34m
RESET=\033[0m