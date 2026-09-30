#include "cli.h"

/**
 * cmd_load - loads a Blockchain from a file, overriding the local one
 *
 * @state: CLI state
 * @path:  path to the serialized Blockchain
 * @arg2:  unused
 *
 * Return: 0 on success, -1 on failure
 */
int cmd_load(state_t *state, char *path, char *arg2)
{
	blockchain_t *blockchain;

	(void)arg2;
	if (!path)
	{
		printf("Usage: load <path>\n");
		return (-1);
	}
	blockchain = blockchain_deserialize(path);
	if (!blockchain)
	{
		printf("Failed to load Blockchain from '%s' ", path);
		printf("(file missing or invalid format)\n");
		return (-1);
	}
	blockchain_destroy(state->blockchain);
	state->blockchain = blockchain;
	printf("Blockchain loaded from '%s' (%d blocks)\n", path,
		llist_size(blockchain->chain));
	return (0);
}

/**
 * cmd_save - saves the local Blockchain into a file (overwritten)
 *
 * @state: CLI state
 * @path:  path of the output file
 * @arg2:  unused
 *
 * Return: 0 on success, -1 on failure
 */
int cmd_save(state_t *state, char *path, char *arg2)
{
	(void)arg2;
	if (!path)
	{
		printf("Usage: save <path>\n");
		return (-1);
	}
	if (blockchain_serialize(state->blockchain, path) == -1)
	{
		printf("Failed to save Blockchain to '%s'\n", path);
		return (-1);
	}
	printf("Blockchain saved to '%s' (%d blocks)\n", path,
		llist_size(state->blockchain->chain));
	return (0);
}
