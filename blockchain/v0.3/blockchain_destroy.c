#include "blockchain.h"

/**
 * blockchain_destroy - deletes a Blockchain, its Blocks and unspent list
 *
 * @blockchain: pointer to the Blockchain to delete
 */
void blockchain_destroy(blockchain_t *blockchain)
{
	if (!blockchain)
		return;
	if (blockchain->chain)
		llist_destroy(blockchain->chain, 1, (node_dtor_t)block_destroy);
	if (blockchain->unspent)
		llist_destroy(blockchain->unspent, 1, free);
	free(blockchain);
}
