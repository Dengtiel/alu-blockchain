#include "blockchain.h"

/**
 * blockchain_destroy - deletes a Blockchain and all the Blocks it contains
 *
 * @blockchain: pointer to the Blockchain to delete
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;
	llist_destroy(blockchain->chain, 1, (node_dtor_t)block_destroy);
	free(blockchain);
}
