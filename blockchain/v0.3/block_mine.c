#include "blockchain.h"

/**
 * block_mine - mines a Block to insert it in the Blockchain
 *
 * @block: pointer to the Block to be mined
 */
void block_mine(block_t *block)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];

	if (!block)
		return;
	for (block->info.nonce = 0; ; block->info.nonce++)
	{
		if (block_hash(block, hash) &&
			hash_matches_difficulty(hash, block->info.difficulty))
			break;
	}
	memcpy(block->hash, hash, SHA256_DIGEST_LENGTH);
}
