#include "transaction.h"

/**
 * is_spent - checks whether an unspent output is consumed by a transaction
 *
 * @unspent:      unspent transaction output to check
 * @transactions: list of validated transactions
 *
 * Return: 1 if an input consumes it, 0 otherwise
 */
static int is_spent(unspent_tx_out_t const *unspent, llist_t *transactions)
{
	int i, j, nb_tx, nb_in;
	transaction_t *tx;
	tx_in_t *in;

	nb_tx = llist_size(transactions);
	for (i = 0; i < nb_tx; i++)
	{
		tx = llist_get_node_at(transactions, i);
		nb_in = llist_size(tx->inputs);
		for (j = 0; j < nb_in; j++)
		{
			in = llist_get_node_at(tx->inputs, j);
			if (!memcmp(in->block_hash, unspent->block_hash,
				SHA256_DIGEST_LENGTH) &&
				!memcmp(in->tx_id, unspent->tx_id,
				SHA256_DIGEST_LENGTH) &&
				!memcmp(in->tx_out_hash, unspent->out.hash,
				SHA256_DIGEST_LENGTH))
				return (1);
		}
	}
	return (0);
}

/**
 * push - appends an unspent output to a list, freeing it on failure
 *
 * @list:    list to append to
 * @unspent: unspent transaction output to append (may be NULL)
 *
 * Return: 0 on success, -1 on failure
 */
static int push(llist_t *list, unspent_tx_out_t *unspent)
{
	if (!unspent || llist_add_node(list, unspent, ADD_NODE_REAR) == -1)
	{
		free(unspent);
		return (-1);
	}
	return (0);
}

/**
 * add_outputs - appends every output of a transaction as unspent
 *
 * @list:       list to append to
 * @tx:         transaction whose outputs to add
 * @block_hash: hash of the Block containing the transaction
 *
 * Return: 0 on success, -1 on failure
 */
static int add_outputs(llist_t *list, transaction_t *tx,
	uint8_t block_hash[SHA256_DIGEST_LENGTH])
{
	int i, size = llist_size(tx->outputs);
	tx_out_t *out;

	for (i = 0; i < size; i++)
	{
		out = llist_get_node_at(tx->outputs, i);
		if (push(list, unspent_tx_out_create(block_hash, tx->id, out)) == -1)
			return (-1);
	}
	return (0);
}

/**
 * update_unspent - updates the list of all unspent transaction outputs
 *
 * @transactions: list of validated transactions
 * @block_hash:   hash of the validated Block containing @transactions
 * @all_unspent:  current list of unspent transaction outputs
 *
 * Return: the new list of unspent transaction outputs, or NULL on failure
 */
llist_t *update_unspent(llist_t *transactions,
	uint8_t block_hash[SHA256_DIGEST_LENGTH], llist_t *all_unspent)
{
	llist_t *new_list;
	unspent_tx_out_t *u;
	int i, size;

	if (!transactions || !block_hash || !all_unspent)
		return (NULL);
	new_list = llist_create(MT_SUPPORT_FALSE);
	if (!new_list)
		return (NULL);
	size = llist_size(all_unspent);
	for (i = 0; i < size; i++)
	{
		u = llist_get_node_at(all_unspent, i);
		if (!is_spent(u, transactions) && push(new_list,
			unspent_tx_out_create(u->block_hash, u->tx_id, &u->out)) == -1)
		{
			llist_destroy(new_list, 1, free);
			return (NULL);
		}
	}
	size = llist_size(transactions);
	for (i = 0; i < size; i++)
	{
		if (add_outputs(new_list, llist_get_node_at(transactions, i),
			block_hash) == -1)
		{
			llist_destroy(new_list, 1, free);
			return (NULL);
		}
	}
	llist_destroy(all_unspent, 1, free);
	return (new_list);
}
