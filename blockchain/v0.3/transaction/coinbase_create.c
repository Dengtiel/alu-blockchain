#include "transaction.h"

/**
 * cb_fail - frees a partially built coinbase transaction
 *
 * @tx: transaction to free
 *
 * Return: always NULL
 */
static transaction_t *cb_fail(transaction_t *tx)
{
	if (tx)
	{
		if (tx->inputs)
			llist_destroy(tx->inputs, 1, free);
		if (tx->outputs)
			llist_destroy(tx->outputs, 1, free);
		free(tx);
	}
	return (NULL);
}

/**
 * coinbase_create - creates a coinbase transaction
 *
 * @receiver:    public key of the miner, who receives the coinbase coins
 * @block_index: index of the Block the coinbase transaction belongs to
 *
 * Return: pointer to the created transaction, or NULL on failure
 */
transaction_t *coinbase_create(EC_KEY const *receiver, uint32_t block_index)
{
	transaction_t *tx;
	tx_in_t *in;
	tx_out_t *out;
	uint8_t pub[EC_PUB_LEN];

	if (!receiver || !ec_to_pub(receiver, pub))
		return (NULL);
	tx = calloc(1, sizeof(*tx));
	if (!tx)
		return (NULL);
	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	tx->outputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx->inputs || !tx->outputs)
		return (cb_fail(tx));
	in = calloc(1, sizeof(*in));
	if (!in || llist_add_node(tx->inputs, in, ADD_NODE_REAR) == -1)
	{
		free(in);
		return (cb_fail(tx));
	}
	memcpy(in->tx_out_hash, &block_index, sizeof(block_index));
	out = tx_out_create(COINBASE_AMOUNT, pub);
	if (!out || llist_add_node(tx->outputs, out, ADD_NODE_REAR) == -1)
	{
		free(out);
		return (cb_fail(tx));
	}
	if (!transaction_hash(tx, tx->id))
		return (cb_fail(tx));
	return (tx);
}
