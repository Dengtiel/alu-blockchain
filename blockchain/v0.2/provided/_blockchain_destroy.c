#include "../blockchain.h"

/**
 * _blockchain_destroy - deletes a Blockchain and all its Blocks
 *
 * @blockchain: pointer to the Blockchain to delete
 */
void _blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;
	llist_destroy(blockchain->chain, 1, NULL);
	free(blockchain);
}
