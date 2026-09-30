#include "cli.h"

/**
 * print_hex - prints a buffer in hexadecimal
 *
 * @buf: buffer to print
 * @len: number of bytes
 */
void print_hex(uint8_t const *buf, size_t len)
{
	size_t i;

	for (i = 0; buf && i < len; i++)
		printf("%02x", buf[i]);
}

/**
 * wallet_balance - sums the unspent outputs owned by a public key
 *
 * @unspent: list of unspent transaction outputs
 * @pub:     owner's public key
 *
 * Return: total amount of coins
 */
static unsigned long wallet_balance(llist_t *unspent,
	uint8_t const pub[EC_PUB_LEN])
{
	unsigned long total = 0;
	int i, size = llist_size(unspent);
	unspent_tx_out_t *u;

	for (i = 0; i < size; i++)
	{
		u = llist_get_node_at(unspent, i);
		if (!memcmp(u->out.pub, pub, EC_PUB_LEN))
			total += u->out.amount;
	}
	return (total);
}

/**
 * cmd_info - displays information about the Blockchain and the wallet
 *
 * @state: CLI state
 * @arg1:  unused
 * @arg2:  unused
 *
 * Return: always 0
 */
int cmd_info(state_t *state, char *arg1, char *arg2)
{
	uint8_t pub[EC_PUB_LEN];
	block_t *last;

	(void)arg1;
	(void)arg2;
	printf("Blocks:               %d\n", llist_size(state->blockchain->chain));
	printf("Unspent outputs:      %d\n",
		llist_size(state->blockchain->unspent));
	printf("Pending transactions: %d\n", llist_size(state->tx_pool));
	printf("Next difficulty:      %u\n",
		blockchain_difficulty(state->blockchain));
	last = llist_get_tail(state->blockchain->chain);
	if (last)
	{
		printf("Last Block:           #%u ", last->info.index);
		print_hex(last->hash, SHA256_DIGEST_LENGTH);
		printf("\n");
	}
	if (ec_to_pub(state->wallet, pub))
	{
		printf("Wallet balance:       %lu coins (confirmed)\n",
			wallet_balance(state->blockchain->unspent, pub));
		printf("Wallet address:       ");
		print_hex(pub, EC_PUB_LEN);
		printf("\n");
	}
	return (0);
}
