#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/**
 * _print_hex_buffer - Prints a buffer as a sequence of hexadecimal values
 * @buf: Buffer to print
 * @len: Number of bytes to print in @buf
 */
void _print_hex_buffer(uint8_t const *buf, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		printf("%02x", buf[i]);
}
