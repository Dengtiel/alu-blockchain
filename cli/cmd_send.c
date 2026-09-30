#include <ctype.h>
#include "cli.h"

/**
 * hex_to_pub - converts a hexadecimal address into a public key buffer
 *
 * @hex: hexadecimal string (EC_PUB_LEN * 2 characters)
 * @pub: buffer to fill
 *
 * Return: 0 on success, -1 on invalid input
 */
static int hex_to_pub(char const *hex, uint8_t pub[EC_PUB_LEN])
{
	char pair[3];
	size_t i;

	if (strlen(hex) != EC_PUB_LEN * 2)
		return (-1);
	pair[2] = '\0';
	for (i = 0; i < EC_PUB_LEN; i++)
	{
		pair[0] = hex[i * 2];
		pair[1] = hex[i * 2 + 1];
		if (!isxdigit((unsigned char)pair[0]) ||
			!isxdigit((unsigned char)pair[1]))
			return (-1);
		pub[i] = (uint8_t)strtoul(pair, NULL, 16);
	}
	return (0);
}

/**
 * spent_by_pool - checks whether a pending transaction uses an output
 *
 * @u:    unspent transaction output
 * @pool: transaction pool
 *
 * Return: 1 if already used by a pending transaction, 0 otherwise
 */
static int spent_by_pool(unspent_tx_out_t const *u, llist_t *pool)
{
	int i, j, nb_tx = llist_size(pool), nb_in;
	transaction_t *tx;
	tx_in_t *in;

	for (i = 0; i < nb_tx; i++)
	{
		tx = llist_get_node_at(pool, i);
		nb_in = llist_size(tx->inputs);
		for (j = 0; j < nb_in; j++)
		{
			in = llist_get_node_at(tx->inputs, j);
			if (!memcmp(in->block_hash, u->block_hash, SHA256_DIGEST_LENGTH) &&
				!memcmp(in->tx_id, u->tx_id, SHA256_DIGEST_LENGTH) &&
				!memcmp(in->tx_out_hash, u->out.hash, SHA256_DIGEST_LENGTH))
				return (1);
		}
	}
	return (0);
}

/**
 * available_unspent - lists unspent outputs not used by the pool
 *
 * @state: CLI state
 *
 * Return: list of pointers (not copies), or NULL on failure
 */
static llist_t *available_unspent(state_t *state)
{
	llist_t *available = llist_create(MT_SUPPORT_FALSE);
	int i, size = llist_size(state->blockchain->unspent);
	unspent_tx_out_t *u;

	if (!available)
		return (NULL);
	for (i = 0; i < size; i++)
	{
		u = llist_get_node_at(state->blockchain->unspent, i);
		if (!spent_by_pool(u, state->tx_pool) &&
			llist_add_node(available, u, ADD_NODE_REAR) == -1)
		{
			llist_destroy(available, 0, NULL);
			return (NULL);
		}
	}
	return (available);
}

/**
 * cmd_send - creates a transaction and adds it to the transaction pool
 *
 * @state:      CLI state
 * @amount_str: number of coins to send
 * @address:    EC public key of the receiver (hexadecimal)
 *
 * Return: 0 on success, -1 on failure
 */
int cmd_send(state_t *state, char *amount_str, char *address)
{
	uint8_t pub[EC_PUB_LEN];
	EC_KEY *receiver;
	llist_t *available;
	transaction_t *tx = NULL;
	unsigned long amount;
	char *end;

	if (!amount_str || !address)
		return (printf("Usage: send <amount> <address>\n"), -1);
	amount = strtoul(amount_str, &end, 10);
	if (*end || amount_str[0] == '-' || !amount || amount > UINT32_MAX)
		return (printf("Invalid amount '%s'\n", amount_str), -1);
	receiver = hex_to_pub(address, pub) == -1 ? NULL : ec_from_pub(pub);
	if (!receiver)
		return (printf("Invalid address\n"), -1);
	available = available_unspent(state);
	if (available)
		tx = transaction_create(state->wallet, receiver, (uint32_t)amount,
			available);
	EC_KEY_free(receiver);
	if (available)
		llist_destroy(available, 0, NULL);
	if (!tx)
		return (printf("Failed to create transaction (not enough coins?)\n"), -1);
	if (!transaction_is_valid(tx, state->blockchain->unspent) ||
		llist_add_node(state->tx_pool, tx, ADD_NODE_REAR) == -1)
	{
		transaction_destroy(tx);
		return (printf("Transaction is invalid\n"), -1);
	}
	printf("Transaction added to the pool: ");
	print_hex(tx->id, SHA256_DIGEST_LENGTH);
	printf("\n");
	return (0);
}
