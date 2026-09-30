#include "blockchain.h"

#define TX_IN_SIZE 169
#define TX_OUT_SIZE 101
#define UNSPENT_SIZE 165

/**
 * write_header - writes the file header
 *
 * @fp:         open file stream
 * @nb_blocks:  number of blocks in the Blockchain
 * @nb_unspent: number of unspent transaction outputs
 *
 * Return: 0 on success, -1 on failure
 */
static int write_header(FILE *fp, uint32_t nb_blocks, uint32_t nb_unspent)
{
	uint8_t endian = _get_endianness();

	if (fwrite(HBLK_MAGIC, 4, 1, fp) != 1 ||
		fwrite(HBLK_VERSION, 3, 1, fp) != 1 ||
		fwrite(&endian, 1, 1, fp) != 1 ||
		fwrite(&nb_blocks, 4, 1, fp) != 1 ||
		fwrite(&nb_unspent, 4, 1, fp) != 1)
		return (-1);
	return (0);
}

/**
 * write_tx - writes a single transaction
 *
 * @fp: open file stream
 * @tx: transaction to serialize
 *
 * Return: 0 on success, -1 on failure
 */
static int write_tx(FILE *fp, transaction_t const *tx)
{
	int32_t nb_in = llist_size(tx->inputs), nb_out = llist_size(tx->outputs);
	int32_t i;

	if (fwrite(tx->id, SHA256_DIGEST_LENGTH, 1, fp) != 1 ||
		fwrite(&nb_in, 4, 1, fp) != 1 || fwrite(&nb_out, 4, 1, fp) != 1)
		return (-1);
	for (i = 0; i < nb_in; i++)
		if (fwrite(llist_get_node_at(tx->inputs, i), TX_IN_SIZE, 1, fp) != 1)
			return (-1);
	for (i = 0; i < nb_out; i++)
		if (fwrite(llist_get_node_at(tx->outputs, i), TX_OUT_SIZE, 1,
			fp) != 1)
			return (-1);
	return (0);
}

/**
 * write_block - writes a single Block and its transactions
 *
 * @fp:    open file stream
 * @block: Block to serialize
 *
 * Return: 0 on success, -1 on failure
 */
static int write_block(FILE *fp, block_t const *block)
{
	int32_t nb_tx, i;

	if (!block)
		return (-1);
	nb_tx = block->transactions ? llist_size(block->transactions) : -1;
	if (fwrite(&block->info, sizeof(block->info), 1, fp) != 1 ||
		fwrite(&block->data.len, 4, 1, fp) != 1 ||
		fwrite(block->data.buffer, 1, block->data.len, fp) !=
		block->data.len ||
		fwrite(block->hash, SHA256_DIGEST_LENGTH, 1, fp) != 1 ||
		fwrite(&nb_tx, 4, 1, fp) != 1)
		return (-1);
	for (i = 0; i < nb_tx; i++)
		if (write_tx(fp, llist_get_node_at(block->transactions, i)) == -1)
			return (-1);
	return (0);
}

/**
 * write_unspent - writes the list of unspent transaction outputs
 *
 * @fp:      open file stream
 * @unspent: list of unspent transaction outputs
 *
 * Return: 0 on success, -1 on failure
 */
static int write_unspent(FILE *fp, llist_t *unspent)
{
	int i, size = llist_size(unspent);

	for (i = 0; i < size; i++)
		if (fwrite(llist_get_node_at(unspent, i), UNSPENT_SIZE, 1, fp) != 1)
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
	int nb_blocks, nb_unspent, i, ret = 0;

	if (!blockchain || !path)
		return (-1);
	nb_blocks = llist_size(blockchain->chain);
	nb_unspent = llist_size(blockchain->unspent);
	if (nb_blocks < 0)
		return (-1);
	if (nb_unspent < 0)
		nb_unspent = 0;
	fp = fopen(path, "wb");
	if (!fp)
		return (-1);
	if (write_header(fp, nb_blocks, nb_unspent) == -1)
		ret = -1;
	for (i = 0; ret == 0 && i < nb_blocks; i++)
		ret = write_block(fp, llist_get_node_at(blockchain->chain, i));
	if (ret == 0)
		ret = write_unspent(fp, blockchain->unspent);
	fclose(fp);
	return (ret);
}
