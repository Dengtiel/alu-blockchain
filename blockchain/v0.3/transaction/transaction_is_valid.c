#include "transaction.h"

/**
 * find_unspent - finds the unspent output referenced by an input
 *
 * @all_unspent: list of all unspent transaction outputs
 * @in:          transaction input
 *
 * Return: pointer to the matching unspent output, or NULL if not found
 */
static unspent_tx_out_t *find_unspent(llist_t *all_unspent,
	tx_in_t const *in)
{
	int i, size = llist_size(all_unspent);
	unspent_tx_out_t *unspent;

	for (i = 0; i < size; i++)
	{
		unspent = llist_get_node_at(all_unspent, i);
		if (!memcmp(unspent->block_hash, in->block_hash,
			SHA256_DIGEST_LENGTH) &&
			!memcmp(unspent->tx_id, in->tx_id, SHA256_DIGEST_LENGTH) &&
			!memcmp(unspent->out.hash, in->tx_out_hash,
			SHA256_DIGEST_LENGTH))
			return (unspent);
	}
	return (NULL);
}

/**
 * check_input - verifies a single transaction input
 *
 * @in:          transaction input to verify
 * @tx_id:       ID of the transaction the input belongs to
 * @all_unspent: list of all unspent transaction outputs
 * @total:       running total of input amounts, updated on success
 *
 * Return: 1 if the input is valid, 0 otherwise
 */
static int check_input(tx_in_t const *in,
	uint8_t const tx_id[SHA256_DIGEST_LENGTH], llist_t *all_unspent,
	uint64_t *total)
{
	unspent_tx_out_t *unspent;
	EC_KEY *key;
	int valid;

	unspent = find_unspent(all_unspent, in);
	if (!unspent)
		return (0);
	key = ec_from_pub(unspent->out.pub);
	if (!key)
		return (0);
	valid = ec_verify(key, tx_id, SHA256_DIGEST_LENGTH, &in->sig);
	EC_KEY_free(key);
	if (!valid)
		return (0);
	*total += unspent->out.amount;
	return (1);
}

/**
 * transaction_is_valid - checks whether a transaction is valid
 *
 * @transaction: transaction to verify
 * @all_unspent: list of all unspent transaction outputs to date
 *
 * Return: 1 if the transaction is valid, 0 otherwise
 */
int transaction_is_valid(transaction_t const *transaction,
	llist_t *all_unspent)
{
	uint8_t hash[SHA256_DIGEST_LENGTH];
	uint64_t in_total = 0, out_total = 0;
	int i, size;
	tx_out_t *out;

	if (!transaction || !all_unspent)
		return (0);
	if (!transaction_hash(transaction, hash) ||
		memcmp(hash, transaction->id, SHA256_DIGEST_LENGTH))
		return (0);
	size = llist_size(transaction->inputs);
	for (i = 0; i < size; i++)
	{
		if (!check_input(llist_get_node_at(transaction->inputs, i),
			transaction->id, all_unspent, &in_total))
			return (0);
	}
	size = llist_size(transaction->outputs);
	for (i = 0; i < size; i++)
	{
		out = llist_get_node_at(transaction->outputs, i);
		out_total += out->amount;
	}
	return (in_total == out_total);
}
