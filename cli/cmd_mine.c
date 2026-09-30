#include "cli.h"

/**
 * copy_unspent - duplicates a list of unspent transaction outputs
 *
 * @unspent: list to duplicate
 *
 * Return: the new list, or NULL on failure
 */
static llist_t *copy_unspent(llist_t *unspent)
{
	llist_t *copy = llist_create(MT_SUPPORT_FALSE);
	unspent_tx_out_t *u, *dup;
	int i, size = llist_size(unspent);

	if (!copy)
		return (NULL);
	for (i = 0; i < size; i++)
	{
		u = llist_get_node_at(unspent, i);
		dup = unspent_tx_out_create(u->block_hash, u->tx_id, &u->out);
		if (!dup || llist_add_node(copy, dup, ADD_NODE_REAR) == -1)
		{
			free(dup);
			llist_destroy(copy, 1, free);
			return (NULL);
		}
	}
	return (copy);
}

/**
 * accept_tx - validates a transaction and adds it to the Block
 *
 * @block:   Block being built
 * @tx:      transaction to process
 * @working: working list of unspent outputs, updated on success
 *
 * Return: 0 if the transaction was included, -1 otherwise
 */
static int accept_tx(block_t *block, transaction_t *tx, llist_t **working)
{
	uint8_t pending_hash[SHA256_DIGEST_LENGTH];
	llist_t *single, *updated;

	if (!transaction_is_valid(tx, *working))
		return (-1);
	memset(pending_hash, 0, sizeof(pending_hash));
	single = llist_create(MT_SUPPORT_FALSE);
	if (!single || llist_add_node(single, tx, ADD_NODE_REAR) == -1)
	{
		if (single)
			llist_destroy(single, 0, NULL);
		return (-1);
	}
	updated = update_unspent(single, pending_hash, *working);
	llist_destroy(single, 0, NULL);
	if (!updated)
		return (-1);
	*working = updated;
	return (llist_add_node(block->transactions, tx, ADD_NODE_REAR));
}

/**
 * process_pool - moves valid pool transactions into the Block, 1 by 1
 *
 * @state: CLI state
 * @block: Block being built
 *
 * Return: 0 on success, -1 on failure
 */
static int process_pool(state_t *state, block_t *block)
{
	llist_t *working = copy_unspent(state->blockchain->unspent);
	transaction_t *tx;
	int accepted = 0, rejected = 0;

	if (!working)
		return (-1);
	for (tx = llist_pop(state->tx_pool); tx; tx = llist_pop(state->tx_pool))
	{
		if (accept_tx(block, tx, &working) == 0)
			accepted++;
		else
		{
			transaction_destroy(tx);
			rejected++;
		}
	}
	llist_destroy(working, 1, free);
	printf("Transaction pool: %d included, %d rejected\n", accepted, rejected);
	return (0);
}

/**
 * finalize_block - mines, validates and appends a Block
 *
 * @state: CLI state
 * @block: Block to finalize
 * @prev:  previous Block in the chain
 *
 * Return: 0 on success, -1 on failure
 */
static int finalize_block(state_t *state, block_t *block, block_t *prev)
{
	llist_t *updated;

	block_mine(block);
	if (block_is_valid(block, prev, state->blockchain->unspent) != 0)
	{
		printf("Mined Block is invalid, discarded\n");
		block_destroy(block);
		return (-1);
	}
	updated = update_unspent(block->transactions, block->hash,
		state->blockchain->unspent);
	if (!updated)
	{
		printf("Failed to update unspent outputs\n");
		block_destroy(block);
		return (-1);
	}
	state->blockchain->unspent = updated;
	llist_add_node(state->blockchain->chain, block, ADD_NODE_REAR);
	printf("Block #%u mined (difficulty %u, %d transaction(s)): ",
		block->info.index, block->info.difficulty,
		llist_size(block->transactions));
	print_hex(block->hash, SHA256_DIGEST_LENGTH);
	printf("\n");
	return (0);
}

/**
 * cmd_mine - mines a Block with a coinbase and the pooled transactions
 *
 * @state: CLI state
 * @arg1:  unused
 * @arg2:  unused
 *
 * Return: 0 on success, -1 on failure
 */
int cmd_mine(state_t *state, char *arg1, char *arg2)
{
	block_t *prev, *block;
	transaction_t *coinbase;

	(void)arg1;
	(void)arg2;
	prev = llist_get_tail(state->blockchain->chain);
	block = block_create(prev, (int8_t const *)"", 0);
	if (!block)
		return (printf("Failed to create Block\n"), -1);
	block->info.difficulty = blockchain_difficulty(state->blockchain);
	coinbase = coinbase_create(state->wallet, block->info.index);
	if (!coinbase ||
		llist_add_node(block->transactions, coinbase, ADD_NODE_FRONT) == -1)
	{
		transaction_destroy(coinbase);
		block_destroy(block);
		return (printf("Failed to create coinbase transaction\n"), -1);
	}
	if (process_pool(state, block) == -1)
	{
		block_destroy(block);
		return (printf("Failed to process the transaction pool\n"), -1);
	}
	return (finalize_block(state, block, prev));
}
