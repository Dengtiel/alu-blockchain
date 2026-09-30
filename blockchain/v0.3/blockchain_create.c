#include "blockchain.h"

/**
 * genesis_create - creates the Genesis Block
 *
 * Return: pointer to the Genesis Block, or NULL on failure
 */
static block_t *genesis_create(void)
{
	block_t *genesis;

	genesis = calloc(1, sizeof(*genesis));
	if (!genesis)
		return (NULL);
	genesis->info.timestamp = GENESIS_TIMESTAMP;
	memcpy(genesis->data.buffer, GENESIS_DATA, GENESIS_DATA_LEN);
	genesis->data.len = GENESIS_DATA_LEN;
	genesis->transactions = NULL;
	memcpy(genesis->hash, GENESIS_HASH, SHA256_DIGEST_LENGTH);
	return (genesis);
}

/**
 * blockchain_create - creates and initializes a Blockchain
 *
 * Return: pointer to the new Blockchain, or NULL on failure
 */
blockchain_t *blockchain_create(void)
{
	blockchain_t *blockchain;
	block_t *genesis;

	blockchain = calloc(1, sizeof(*blockchain));
	if (!blockchain)
		return (NULL);
	genesis = genesis_create();
	blockchain->chain = llist_create(MT_SUPPORT_TRUE);
	blockchain->unspent = llist_create(MT_SUPPORT_TRUE);
	if (!genesis || !blockchain->chain || !blockchain->unspent ||
		llist_add_node(blockchain->chain, genesis, ADD_NODE_REAR) == -1)
	{
		if (blockchain->chain)
			llist_destroy(blockchain->chain, 0, NULL);
		if (blockchain->unspent)
			llist_destroy(blockchain->unspent, 0, NULL);
		free(genesis);
		free(blockchain);
		return (NULL);
	}
	return (blockchain);
}
