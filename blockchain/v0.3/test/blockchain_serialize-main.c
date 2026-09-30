#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "blockchain.h"

void _blockchain_print_brief(blockchain_t const *blockchain);

/**
 * _add_block - mines a Block and appends it to a Blockchain
 *
 * @bc:    Blockchain
 * @prev:  previous Block
 * @data:  Block data
 * @miner: miner key
 * @tx:    optional extra transaction (may be NULL)
 *
 * Return: the new Block, or prev on failure
 */
static block_t *_add_block(blockchain_t *bc, block_t *prev, char const *data,
	EC_KEY *miner, transaction_t *tx)
{
	block_t *block;

	block = block_create(prev, (int8_t *)data, (uint32_t)strlen(data));
	block->info.difficulty = 16;
	llist_add_node(block->transactions,
		coinbase_create(miner, block->info.index), ADD_NODE_FRONT);
	if (tx)
		llist_add_node(block->transactions, tx, ADD_NODE_REAR);
	block_mine(block);
	if (block_is_valid(block, prev, bc->unspent) != 0)
	{
		fprintf(stderr, "Invalid Block %u\n", block->info.index);
		block_destroy(block);
		return (prev);
	}
	bc->unspent = update_unspent(block->transactions, block->hash,
		bc->unspent);
	llist_add_node(bc->chain, block, ADD_NODE_REAR);
	return (block);
}

/**
 * main - Entry point
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	blockchain_t *bc;
	block_t *block;
	EC_KEY *miner, *receiver;
	transaction_t *tx;

	miner = ec_create();
	receiver = ec_create();
	bc = blockchain_create();
	block = llist_get_head(bc->chain);
	block = _add_block(bc, block, "Holberton", miner, NULL);
	block = _add_block(bc, block, "School", miner, NULL);
	tx = transaction_create(miner, receiver, 30, bc->unspent);
	block = _add_block(bc, block, "of", miner, tx);

	if (blockchain_serialize(bc, "save.hblk") == -1)
		fprintf(stderr, "Serialization failed\n");
	_blockchain_print_brief(bc);
	printf("Unspent: %d\n", llist_size(bc->unspent));

	blockchain_destroy(bc);
	EC_KEY_free(miner);
	EC_KEY_free(receiver);
	return (EXIT_SUCCESS);
}
