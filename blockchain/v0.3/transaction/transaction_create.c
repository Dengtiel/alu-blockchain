#include "transaction.h"

/**
 * select_inputs - creates inputs from the sender's unspent outputs
 *
 * @inputs:      list to add the created inputs to
 * @all_unspent: list of all unspent transaction outputs
 * @pub:         sender's public key
 * @amount:      amount the selected outputs must cover
 *
 * Return: total amount of the selected unspent outputs
 */
static uint64_t select_inputs(llist_t *inputs, llist_t *all_unspent,
	uint8_t const pub[EC_PUB_LEN], uint32_t amount)
{
	int i, size = llist_size(all_unspent);
	uint64_t total = 0;
	unspent_tx_out_t *unspent;
	tx_in_t *in;

	for (i = 0; i < size && total < amount; i++)
	{
		unspent = llist_get_node_at(all_unspent, i);
		if (memcmp(unspent->out.pub, pub, EC_PUB_LEN))
			continue;
		in = tx_in_create(unspent);
		if (!in || llist_add_node(inputs, in, ADD_NODE_REAR) == -1)
		{
			free(in);
			return (0);
		}
		total += unspent->out.amount;
	}
	return (total);
}

/**
 * add_outputs - creates the transaction outputs
 *
 * @outputs:      list to add the created outputs to
 * @sender_pub:   sender's public key (receives the leftover)
 * @receiver_pub: receiver's public key
 * @amount:       amount to send
 * @total:        total amount of the selected inputs
 *
 * Return: 0 on success, -1 on failure
 */
static int add_outputs(llist_t *outputs, uint8_t const sender_pub[EC_PUB_LEN],
	uint8_t const receiver_pub[EC_PUB_LEN], uint32_t amount, uint64_t total)
{
	tx_out_t *out;

	out = tx_out_create(amount, receiver_pub);
	if (!out || llist_add_node(outputs, out, ADD_NODE_REAR) == -1)
	{
		free(out);
		return (-1);
	}
	if (total > amount)
	{
		out = tx_out_create((uint32_t)(total - amount), sender_pub);
		if (!out || llist_add_node(outputs, out, ADD_NODE_REAR) == -1)
		{
			free(out);
			return (-1);
		}
	}
	return (0);
}

/**
 * sign_inputs - signs every input of a transaction
 *
 * @tx:          transaction whose inputs to sign
 * @sender:      sender's private key
 * @all_unspent: list of all unspent transaction outputs
 *
 * Return: 0 on success, -1 on failure
 */
static int sign_inputs(transaction_t *tx, EC_KEY const *sender,
	llist_t *all_unspent)
{
	int i, size = llist_size(tx->inputs);

	for (i = 0; i < size; i++)
	{
		if (!tx_in_sign(llist_get_node_at(tx->inputs, i), tx->id,
			sender, all_unspent))
			return (-1);
	}
	return (0);
}

/**
 * tx_fail - frees a partially built transaction
 *
 * @tx: transaction to free
 *
 * Return: always NULL
 */
static transaction_t *tx_fail(transaction_t *tx)
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
 * transaction_create - creates a transaction
 *
 * @sender:      private key of the transaction sender
 * @receiver:    public key of the transaction receiver
 * @amount:      amount to send
 * @all_unspent: list of all the unspent outputs to date
 *
 * Return: pointer to the created transaction, or NULL on failure
 */
transaction_t *transaction_create(EC_KEY const *sender,
	EC_KEY const *receiver, uint32_t amount, llist_t *all_unspent)
{
	transaction_t *tx;
	uint8_t sender_pub[EC_PUB_LEN], receiver_pub[EC_PUB_LEN];
	uint64_t total;

	if (!sender || !receiver || !all_unspent)
		return (NULL);
	if (!ec_to_pub(sender, sender_pub) ||
		!ec_to_pub(receiver, receiver_pub))
		return (NULL);
	tx = calloc(1, sizeof(*tx));
	if (!tx)
		return (NULL);
	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	tx->outputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx->inputs || !tx->outputs)
		return (tx_fail(tx));
	total = select_inputs(tx->inputs, all_unspent, sender_pub, amount);
	if (total < amount || llist_size(tx->inputs) < 1)
		return (tx_fail(tx));
	if (add_outputs(tx->outputs, sender_pub, receiver_pub, amount,
		total) == -1)
		return (tx_fail(tx));
	if (!transaction_hash(tx, tx->id) ||
		sign_inputs(tx, sender, all_unspent) == -1)
		return (tx_fail(tx));
	return (tx);
}
