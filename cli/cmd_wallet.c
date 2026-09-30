#include "cli.h"

/**
 * cmd_wallet_load - loads a wallet (EC key pair) from a folder
 *
 * @state: CLI state
 * @path:  folder to load the key pair from
 * @arg2:  unused
 *
 * Return: 0 on success, -1 on failure
 */
int cmd_wallet_load(state_t *state, char *path, char *arg2)
{
	EC_KEY *key;

	(void)arg2;
	if (!path)
	{
		printf("Usage: wallet_load <path>\n");
		return (-1);
	}
	key = ec_load(path);
	if (!key)
	{
		printf("Failed to load wallet from '%s'\n", path);
		return (-1);
	}
	EC_KEY_free(state->wallet);
	state->wallet = key;
	printf("Wallet loaded from '%s'\n", path);
	return (0);
}

/**
 * cmd_wallet_save - saves the wallet (EC key pair) into a folder, as PEM
 *
 * @state: CLI state
 * @path:  folder to save the key pair in (overwritten if it exists)
 * @arg2:  unused
 *
 * Return: 0 on success, -1 on failure
 */
int cmd_wallet_save(state_t *state, char *path, char *arg2)
{
	(void)arg2;
	if (!path)
	{
		printf("Usage: wallet_save <path>\n");
		return (-1);
	}
	if (!ec_save(state->wallet, path))
	{
		printf("Failed to save wallet to '%s'\n", path);
		return (-1);
	}
	printf("Wallet saved to '%s'\n", path);
	return (0);
}
