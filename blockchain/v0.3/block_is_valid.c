#include "blockchain.h"

/**
 * is_genesis - checks whether a Block matches the Genesis Block
 *
 * @block: pointer to the Block to check
 *
 * Return: 1 if it matches, 0 otherwise
 */
static int is_genesis(block_t const *block)
{
	block_info_t info;

	memset(&info, 0, sizeof(info));
	info.timestamp = GENESIS_TIMESTAMP;
	return (!memcmp(&block->info, &info, sizeof(info)) &&
		block->data.len == GENESIS_DATA_LEN &&
		!memcmp(block->data.buffer, GENESIS_DATA, GENESIS_DATA_LEN) &&
		!memcmp(block->hash, GENESIS_HASH, SHA256_DIGEST_LENGTH));
}

/**
 * check_transactions - verifies the list of transactions of a Block
 *
 * @block:       Block whose transactions to verify
 * @all_unspent: list of all unspent transaction outputs
 *
 * Return: 0 if valid, -1 otherwise
 */
static int check_transactions(block_t const *block, llist_t *all_unspent)
{
	int i, size = llist_size(block->transactions);

	if (size < 1)
		return (-1);
	if (!coinbase_is_valid(llist_get_node_at(block->transactions, 0),
		block->info.index))
		return (-1);
	for (i = 1; i < size; i++)
	{
		if (!transaction_is_valid(
			llist_get_node_at(block->transactions, i), all_unspent))
			return (-1);
	}
	return (0);
}

/**
 * block_is_valid - verifies that a Block is valid
 *
 * @block:       pointer to the Block to check
 * @prev_block:  pointer to the previous Block, or NULL if block is first
 * @all_unspent: list of all unspent transaction outputs
 *
 * Return: 0 if valid, -1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block,
	llist_t *all_unspent)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];

	if (!block || (!prev_block && block->info.index != 0))
		return (-1);
	if (block->info.index == 0)
		return (is_genesis(block) ? 0 : -1);
	if (block->info.index != prev_block->info.index + 1)
		return (-1);
	if (block->data.len > BLOCKCHAIN_DATA_MAX)
		return (-1);
	if (!block_hash(prev_block, hash) ||
		memcmp(hash, prev_block->hash, SHA256_DIGEST_LENGTH) ||
		memcmp(hash, block->info.prev_hash, SHA256_DIGEST_LENGTH))
		return (-1);
	if (!block_hash(block, hash) ||
		memcmp(hash, block->hash, SHA256_DIGEST_LENGTH))
		return (-1);
	if (!hash_matches_difficulty(block->hash, block->info.difficulty))
		return (-1);
	return (check_transactions(block, all_unspent));
}
