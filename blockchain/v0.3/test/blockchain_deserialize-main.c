#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "blockchain.h"

void _blockchain_print_brief(blockchain_t const *blockchain);

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	blockchain_t *bc;

	bc = blockchain_deserialize("save.hblk");
	if (!bc)
	{
		fprintf(stderr, "Deserialization failed\n");
		return (EXIT_FAILURE);
	}
	_blockchain_print_brief(bc);
	printf("Unspent: %d\n", llist_size(bc->unspent));
	blockchain_serialize(bc, "save2.hblk");
	blockchain_destroy(bc);
	return (EXIT_SUCCESS);
}
