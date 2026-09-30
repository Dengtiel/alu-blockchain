CC = gcc
BC = ../blockchain/v0.3
CFLAGS = -Wall -Werror -Wextra -pedantic -I. -I$(BC) -I$(BC)/transaction \
	-I$(BC)/provided -I../crypto $(EXTRA)
LDFLAGS = -L$(BC) -L../crypto
LDLIBS = -lhblk_blockchain -lhblk_crypto -lllist -lssl -lcrypto -pthread
SRC = main.c cmd_wallet.c cmd_send.c cmd_mine.c cmd_info.c cmd_chain.c \
	$(BC)/provided/_endianness.c
OBJ = $(SRC:.c=.o)
NAME = cli

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
