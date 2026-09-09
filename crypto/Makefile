CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -I.
LDFLAGS = -lssl -lcrypto

OBJ = sha256.o ec_create.o

all: libhblk_crypto.a

libhblk_crypto.a: $(OBJ)
	ar rcs libhblk_crypto.a $(OBJ)

sha256.o: sha256.c hblk_crypto.h
	$(CC) $(CFLAGS) -c sha256.c -o sha256.o

ec_create.o: ec_create.c hblk_crypto.h
	$(CC) $(CFLAGS) -c ec_create.c -o ec_create.o

sha256-test: test/sha256-main.c provided/_print_hex_buffer.c sha256.c hblk_crypto.h
	$(CC) $(CFLAGS) -o sha256-test test/sha256-main.c provided/_print_hex_buffer.c sha256.c $(LDFLAGS)

ec_create-test: test/ec_create-main.c ec_create.c hblk_crypto.h
	$(CC) $(CFLAGS) -o ec_create-test test/ec_create-main.c ec_create.c $(LDFLAGS)

clean:
	rm -f *.o *.a sha256-test ec_create-test

.PHONY: all clean
