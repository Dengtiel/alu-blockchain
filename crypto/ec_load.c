#include "hblk_crypto.h"

/**
 * ec_load - Loads an EC key pair from disk
 * @folder: Path to the folder from which to load the keys
 *
 * Return: Pointer to the created EC key pair upon success,
 *         or NULL upon failure
 */
EC_KEY *ec_load(char const *folder)
{
	char path[256];
	FILE *fp;
	EC_KEY *key;

	if (folder == NULL)
		return (NULL);

	snprintf(path, sizeof(path), "%s/%s", folder, PRI_FILENAME);
	fp = fopen(path, "r");
	if (fp == NULL)
		return (NULL);

	key = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
	fclose(fp);
	if (key == NULL)
		return (NULL);

	return (key);
}
