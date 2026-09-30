#include "blockchain.h"

/**
 * read_header - reads and checks the file header
 *
 * @fp:        open file stream
 * @endian:    where to store the file endianness
 * @nb_blocks: where to store the number of blocks
 *
 * Return: 0 on success, -1 on failure
 */
static int read_header(FILE *fp, uint8_t *endian, uint32_t *nb_blocks)
{
	char magic[4], version[3];

	if (fread(magic, 4, 1, fp) != 1 || memcmp(magic, HBLK_MAGIC, 4) ||
		fread(version, 3, 1, fp) != 1 ||
		memcmp(version, HBLK_VERSION, 3) ||
		fread(endian, 1, 1, fp) != 1 ||
		fread(nb_blocks, sizeof(*nb_blocks), 1, fp) != 1)
		return (-1);
	return (0);
}

/**
 * read_block - reads a single Block from the file
 *
 * @fp:   open file stream
 * @swap: 1 if multi-byte fields must be byte-swapped, 0 otherwise
 *
 * Return: pointer to the allocated Block, or NULL on failure
 */
static block_t *read_block(FILE *fp, int swap)
{
	block_t *block = calloc(1, sizeof(*block));

	if (!block)
		return (NULL);
	if (fread(&block->info, sizeof(block->info), 1, fp) != 1 ||
		fread(&block->data.len, sizeof(block->data.len), 1, fp) != 1)
	{
		free(block);
		return (NULL);
	}
	if (swap)
	{
		SWAPENDIAN(block->info.index);
		SWAPENDIAN(block->info.difficulty);
		SWAPENDIAN(block->info.timestamp);
		SWAPENDIAN(block->info.nonce);
		SWAPENDIAN(block->data.len);
	}
	if (block->data.len > BLOCKCHAIN_DATA_MAX ||
		fread(block->data.buffer, 1, block->data.len, fp) !=
		block->data.len ||
		fread(block->hash, SHA256_DIGEST_LENGTH, 1, fp) != 1)
	{
		free(block);
		return (NULL);
	}
	return (block);
}

/**
 * fail - cleans up after a failed deserialization
 *
 * @fp:         open file stream
 * @blockchain: partially built Blockchain, or NULL
 *
 * Return: always NULL
 */
static blockchain_t *fail(FILE *fp, blockchain_t *blockchain)
{
	fclose(fp);
	blockchain_destroy(blockchain);
	return (NULL);
}

/**
 * blockchain_deserialize - deserializes a Blockchain from a file
 *
 * @path: path to the file to load the Blockchain from
 *
 * Return: pointer to the deserialized Blockchain, or NULL on failure
 */
blockchain_t *blockchain_deserialize(char const *path)
{
	FILE *fp;
	blockchain_t *blockchain;
	block_t *block;
	uint8_t endian;
	uint32_t nb_blocks, i;
	int swap;

	if (!path)
		return (NULL);
	fp = fopen(path, "rb");
	if (!fp)
		return (NULL);
	if (read_header(fp, &endian, &nb_blocks) == -1)
		return (fail(fp, NULL));
	swap = (endian != _get_endianness());
	if (swap)
		SWAPENDIAN(nb_blocks);
	blockchain = calloc(1, sizeof(*blockchain));
	if (!blockchain)
		return (fail(fp, NULL));
	blockchain->chain = llist_create(MT_SUPPORT_TRUE);
	if (!blockchain->chain)
		return (fail(fp, blockchain));
	for (i = 0; i < nb_blocks; i++)
	{
		block = read_block(fp, swap);
		if (!block || llist_add_node(blockchain->chain, block,
			ADD_NODE_REAR) == -1)
		{
			free(block);
			return (fail(fp, blockchain));
		}
	}
	fclose(fp);
	return (blockchain);
}
