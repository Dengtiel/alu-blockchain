CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -I.
LDFLAGS = -lssl -lcrypto

OBJ = sha256.o ec_create.o ec_to_pub.o ec_from_pub.o ec_save.o

all: libhblk_crypto.a

libhblk_crypto.a: $(OBJ)
	ar rcs libhblk_crypto.a $(OBJ)

sha256.o: sha256.c hblk_crypto.h
	$(CC) $(CFLAGS) -c sha256.c -o sha256.o

ec_create.o: ec_create.c hblk_crypto.h
	$(CC) $(CFLAGS) -c ec_create.c -o ec_create.o

ec_to_pub.o: ec_to_pub.c hblk_crypto.h
	$(CC) $(CFLAGS) -c ec_to_pub.c -o ec_to_pub.o

ec_from_pub.o: ec_from_pub.c hblk_crypto.h
	$(CC) $(CFLAGS) -c ec_from_pub.c -o ec_from_pub.o

ec_save.o: ec_save.c hblk_crypto.h
	$(CC) $(CFLAGS) -c ec_save.c -o ec_save.o

sha256-test: test/sha256-main.c provided/_print_hex_buffer.c sha256.c hblk_crypto.h
	$(CC) $(CFLAGS) -o sha256-test test/sha256-main.c provided/_print_hex_buffer.c sha256.c $(LDFLAGS)

ec_create-test: test/ec_create-main.c ec_create.c hblk_crypto.h
	$(CC) $(CFLAGS) -o ec_create-test test/ec_create-main.c ec_create.c $(LDFLAGS)

ec_to_pub-test: test/ec_to_pub-main.c provided/_print_hex_buffer.c ec_to_pub.c ec_create.c hblk_crypto.h
	$(CC) $(CFLAGS) -o ec_to_pub-test test/ec_to_pub-main.c provided/_print_hex_buffer.c ec_to_pub.c ec_create.c $(LDFLAGS)

ec_from_pub-test: test/ec_from_pub-main.c provided/_print_hex_buffer.c ec_from_pub.c ec_to_pub.c ec_create.c hblk_crypto.h
	$(CC) $(CFLAGS) -o ec_from_pub-test test/ec_from_pub-main.c provided/_print_hex_buffer.c ec_from_pub.c ec_to_pub.c ec_create.c $(LDFLAGS)

ec_save-test: test/ec_save-main.c ec_save.c ec_create.c ec_to_pub.c provided/_print_hex_buffer.c hblk_crypto.h
	$(CC) $(CFLAGS) -o ec_save-test test/ec_save-main.c ec_save.c ec_create.c ec_to_pub.c provided/_print_hex_buffer.c $(LDFLAGS)

clean:
	rm -f *.o *.a sha256-test ec_create-test ec_to_pub-test ec_from_pub-test ec_save-test

.PHONY: all clean
