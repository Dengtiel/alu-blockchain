#include "transaction.h"

#define TX_IN_HASH_LEN (SHA256_DIGEST_LENGTH * 3)

/**
 * transaction_hash - computes the ID (hash) of a transaction
 *
 * @transaction: transaction to compute the hash of
 * @hash_buf:    buffer in which to store the computed hash
 *
 * Return: pointer to hash_buf, or NULL on failure
 */
uint8_t *transaction_hash(transaction_t const *transaction,
	uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	int nb_in, nb_out, i;
	size_t len;
	uint8_t *buf, *p;
	tx_in_t *in;
	tx_out_t *out;

	if (!transaction || !hash_buf)
		return (NULL);
	nb_in = llist_size(transaction->inputs);
	nb_out = llist_size(transaction->outputs);
	if (nb_in < 0 || nb_out < 0)
		return (NULL);
	len = (size_t)nb_in * TX_IN_HASH_LEN +
		(size_t)nb_out * SHA256_DIGEST_LENGTH;
	buf = malloc(len ? len : 1);
	if (!buf)
		return (NULL);
	for (p = buf, i = 0; i < nb_in; i++, p += TX_IN_HASH_LEN)
	{
		in = llist_get_node_at(transaction->inputs, i);
		memcpy(p, in->block_hash, TX_IN_HASH_LEN);
	}
	for (i = 0; i < nb_out; i++, p += SHA256_DIGEST_LENGTH)
	{
		out = llist_get_node_at(transaction->outputs, i);
		memcpy(p, out->hash, SHA256_DIGEST_LENGTH);
	}
	sha256((int8_t const *)buf, len, hash_buf);
	free(buf);
	return (hash_buf);
}
