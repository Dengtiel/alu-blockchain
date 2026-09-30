#include "transaction.h"

/**
 * unspent_tx_out_create - allocates and initializes an unspent tx output
 *
 * @block_hash: hash of the Block containing the referenced output
 * @tx_id:      hash of the transaction containing the referenced output
 * @out:        pointer to the referenced transaction output
 *
 * Return: pointer to the created unspent output, or NULL on failure
 */
unspent_tx_out_t *unspent_tx_out_create(
	uint8_t block_hash[SHA256_DIGEST_LENGTH],
	uint8_t tx_id[SHA256_DIGEST_LENGTH], tx_out_t const *out)
{
	unspent_tx_out_t *unspent;

	if (!block_hash || !tx_id || !out)
		return (NULL);
	unspent = calloc(1, sizeof(*unspent));
	if (!unspent)
		return (NULL);
	memcpy(unspent->block_hash, block_hash, SHA256_DIGEST_LENGTH);
	memcpy(unspent->tx_id, tx_id, SHA256_DIGEST_LENGTH);
	memcpy(&unspent->out, out, sizeof(unspent->out));
	return (unspent);
}
