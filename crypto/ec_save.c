#include "hblk_crypto.h"

/**
 * ec_save - Saves an EC key pair to disk in PEM format
 * @key: Pointer to the EC key pair to save
 * @folder: Path to the folder in which to save the keys
 *
 * Return: 1 upon success, or 0 upon failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	char path[256];
	FILE *fp;
	struct stat st = {0};

	if (key == NULL || folder == NULL)
		return (0);

	if (stat(folder, &st) == -1)
	{
		if (mkdir(folder, 0755) == -1)
			return (0);
	}

	snprintf(path, sizeof(path), "%s/%s", folder, PRI_FILENAME);
	fp = fopen(path, "w");
	if (fp == NULL)
		return (0);
	if (PEM_write_ECPrivateKey(fp, key, NULL, NULL, 0, NULL, NULL) != 1)
	{
		fclose(fp);
		return (0);
	}
	fclose(fp);

	snprintf(path, sizeof(path), "%s/%s", folder, PUB_FILENAME);
	fp = fopen(path, "w");
	if (fp == NULL)
		return (0);
	if (PEM_write_EC_PUBKEY(fp, key) != 1)
	{
		fclose(fp);
		return (0);
	}
	fclose(fp);

	return (1);
}
