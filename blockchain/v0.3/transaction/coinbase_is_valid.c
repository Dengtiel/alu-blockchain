#include "transaction.h"

/**
 * coinbase_is_valid - checks whether a coinbase transaction is valid
 *
 * @coinbase:    coinbase transaction to verify
 * @block_index: index of the Block the coinbase transaction belongs to
 *
 * Return: 1 if the coinbase transaction is valid, 0 otherwise
 */
int coinbase_is_valid(transaction_t const *coinbase, uint32_t block_index)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	uint8_t zero[SHA256_DIGEST_LENGTH];
	sig_t zero_sig;
	tx_in_t *in;
	tx_out_t *out;

	if (!coinbase)
		return (0);
	if (!transaction_hash(coinbase, hash) ||
		memcmp(hash, coinbase->id, SHA256_DIGEST_LENGTH))
		return (0);
	if (llist_size(coinbase->inputs) != 1 ||
		llist_size(coinbase->outputs) != 1)
		return (0);
	in = llist_get_head(coinbase->inputs);
	out = llist_get_head(coinbase->outputs);
	if (!in || !out)
		return (0);
	if (memcmp(in->tx_out_hash, &block_index, sizeof(block_index)))
		return (0);
	memset(zero, 0, sizeof(zero));
	memset(&zero_sig, 0, sizeof(zero_sig));
	if (memcmp(in->block_hash, zero, SHA256_DIGEST_LENGTH) ||
		memcmp(in->tx_id, zero, SHA256_DIGEST_LENGTH) ||
		memcmp(&in->sig, &zero_sig, sizeof(zero_sig)))
		return (0);
	if (out->amount != COINBASE_AMOUNT)
		return (0);
	return (1);
}
