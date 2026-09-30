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
 * block_is_valid - verifies that a Block is valid
 *
 * @block:      pointer to the Block to check
 * @prev_block: pointer to the previous Block, or NULL if block is first
 *
 * Return: 0 if valid, -1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
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
	return (0);
}
