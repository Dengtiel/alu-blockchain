#include "blockchain.h"

/**
 * block_hash - computes the hash of a Block, including its transactions
 *
 * @block:    pointer to the Block to be hashed
 * @hash_buf: buffer to store the resulting hash
 *
 * Return: pointer to hash_buf, or NULL on failure
 */
uint8_t *block_hash(block_t const *block,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t base, len;
	int i, nb_tx;
	uint8_t *buf;
	transaction_t *tx;

	if (!block || !hash_buf)
		return (NULL);
	nb_tx = llist_size(block->transactions);
	if (nb_tx < 0)
		nb_tx = 0;
	base = sizeof(block->info) + block->data.len;
	len = base + (size_t)nb_tx * SHA256_DIGEST_LENGTH;
	buf = malloc(len);
	if (!buf)
		return (NULL);
	memcpy(buf, block, base);
	for (i = 0; i < nb_tx; i++)
	{
		tx = llist_get_node_at(block->transactions, i);
		memcpy(buf + base + i * SHA256_DIGEST_LENGTH, tx->id,
			SHA256_DIGEST_LENGTH);
	}
	sha256((int8_t const *)buf, len, hash_buf);
	free(buf);
	return (hash_buf);
}
