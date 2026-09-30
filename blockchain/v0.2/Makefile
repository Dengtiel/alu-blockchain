CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -I. -I../../crypto
SRC = blockchain_create.c block_create.c block_destroy.c \
	blockchain_destroy.c block_hash.c blockchain_serialize.c \
	blockchain_deserialize.c block_is_valid.c hash_matches_difficulty.c \
	block_mine.c blockchain_difficulty.c provided/_endianness.c
OBJ = $(SRC:.c=.o)
NAME = libhblk_blockchain_v0.2.a

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $@ $^

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
