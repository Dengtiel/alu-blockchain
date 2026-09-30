#include "explorer.h"

/**
 * chain_audit - replays a Blockchain and verifies every Block
 *
 * @bc:       Blockchain to audit
 * @nb_valid: where to store the number of valid Blocks
 *
 * Description: each Block is checked with block_is_valid against the
 * unspent outputs rebuilt from the Blocks before it
 *
 * Return: array of per-Block status (1 valid, 0 invalid), or NULL
 */
int *chain_audit(blockchain_t const *bc, int *nb_valid)
{
	int i, size = llist_size(bc->chain);
	int *status = calloc(size > 0 ? size : 1, sizeof(*status));
	llist_t *unspent = llist_create(MT_SUPPORT_FALSE), *updated;
	block_t *block, *prev = NULL;

	if (!status || !unspent)
	{
		free(status);
		if (unspent)
			llist_destroy(unspent, 0, NULL);
		return (NULL);
	}
	*nb_valid = 0;
	for (i = 0; i < size; i++, prev = block)
	{
		block = llist_get_node_at(bc->chain, i);
		status[i] = (block_is_valid(block, prev, unspent) == 0);
		if (status[i] && block->transactions)
		{
			updated = update_unspent(block->transactions, block->hash,
				unspent);
			if (updated)
				unspent = updated;
		}
		*nb_valid += status[i];
	}
	llist_destroy(unspent, 1, free);
	return (status);
}
