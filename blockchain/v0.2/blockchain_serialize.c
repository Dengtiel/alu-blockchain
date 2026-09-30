#include "blockchain.h"

/**
 * write_header - writes the file header
 *
 * @fp:        open file stream
 * @nb_blocks: number of blocks in the Blockchain
 *
 * Return: 0 on success, -1 on failure
 */
static int write_header(FILE *fp, uint32_t nb_blocks)
{
	uint8_t endian = _get_endianness();

	if (fwrite(HBLK_MAGIC, 4, 1, fp) != 1 ||
		fwrite(HBLK_VERSION, 3, 1, fp) != 1 ||
		fwrite(&endian, 1, 1, fp) != 1 ||
		fwrite(&nb_blocks, sizeof(nb_blocks), 1, fp) != 1)
		return (-1);
	return (0);
}

/**
 * write_block - writes a single Block
 *
 * @fp:    open file stream
 * @block: Block to serialize
 *
 * Return: 0 on success, -1 on failure
 */
static int write_block(FILE *fp, block_t const *block)
{
	if (!block ||
		fwrite(&block->info, sizeof(block->info), 1, fp) != 1 ||
		fwrite(&block->data.len, sizeof(block->data.len), 1, fp) != 1 ||
		fwrite(block->data.buffer, 1, block->data.len, fp) !=
		block->data.len ||
		fwrite(block->hash, SHA256_DIGEST_LENGTH, 1, fp) != 1)
		return (-1);
	return (0);
}

/**
 * blockchain_serialize - serializes a Blockchain into a file
 *
 * @blockchain: pointer to the Blockchain to serialize
 * @path:       path to the output file
 *
 * Return: 0 on success, -1 on failure
 */
int blockchain_serialize(blockchain_t const *blockchain, char const *path)
{
	FILE *fp;
	int size, i;

	if (!blockchain || !path)
		return (-1);
	size = llist_size(blockchain->chain);
	if (size < 0)
		return (-1);
	fp = fopen(path, "wb");
	if (!fp)
		return (-1);
	if (write_header(fp, (uint32_t)size) == -1)
	{
		fclose(fp);
		return (-1);
	}
	for (i = 0; i < size; i++)
	{
		if (write_block(fp, llist_get_node_at(blockchain->chain, i)) == -1)
		{
			fclose(fp);
			return (-1);
		}
	}
	fclose(fp);
	return (0);
}
