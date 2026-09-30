#include "endianness.h"

/**
 * _get_endianness - gets the endianness of the running system
 *
 * Return: 1 for little endian, 2 for big endian
 */
uint8_t _get_endianness(void)
{
	uint16_t x = 1;

	return (*(uint8_t *)&x ? 1 : 2);
}

/**
 * _swap_endian - swaps the byte order of a memory area in place
 *
 * @p:    pointer to the memory area
 * @size: size of the memory area in bytes
 */
void _swap_endian(void *p, size_t size)
{
	uint8_t *bytes = p, tmp;
	size_t i;

	for (i = 0; i < size / 2; i++)
	{
		tmp = bytes[i];
		bytes[i] = bytes[size - 1 - i];
		bytes[size - 1 - i] = tmp;
	}
}
