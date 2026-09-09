CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -I.
LDFLAGS = -lssl -lcrypto

all: libhblk_crypto.a

libhblk_crypto.a: sha256.o
	ar rcs libhblk_crypto.a sha256.o

sha256.o: sha256.c hblk_crypto.h
	$(CC) $(CFLAGS) -c sha256.c -o sha256.o

sha256-test: test/sha256-main.c provided/_print_hex_buffer.c sha256.c hblk_crypto.h
	$(CC) $(CFLAGS) -o sha256-test test/sha256-main.c provided/_print_hex_buffer.c sha256.c $(LDFLAGS)

clean:
	rm -f *.o *.a sha256-test

.PHONY: all clean
