
NAME = ft_select

CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
IFLAGS = -I ./inc/ -I ./libft/includes
LFLAGS = -L ./libft/ -lft -ltermcap

LIBFT = ./libft/libft.a

SRC_DIR = ./src/
SRC_FILES = main.c

OBJ_DIR = ./obj/
OBJ_FILES = $(SRC_FILES:.c=.o)

INC_DIR = ./inc/
INC_FILES = ft_select.h

SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ = $(addprefix $(OBJ_DIR), $(OBJ_FILES))
INC = $(addprefix $(INC_DIR), $(INC_FILES))

.PHONY: all re fclean clean $(LIBFT) 

all: $(OBJ_DIR) $(LIBFT) $(NAME) auteur

$(LIBFT):
	make -C ./libft

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(INC)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(LFLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_DIR)
	make -C ./libft clean

fclean: clean
	rm -rf $(NAME)
	rm -rf $(LIBFT)

re: fclean all
