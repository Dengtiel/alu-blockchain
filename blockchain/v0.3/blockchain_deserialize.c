#include "blockchain.h"

#define TX_IN_SIZE 169
#define TX_OUT_SIZE 101
#define UNSPENT_SIZE 165

/**
 * read_header - reads and checks the file header
 *
 * @fp:         open file stream
 * @swap:       set to 1 if multi-byte fields must be byte-swapped
 * @nb_blocks:  where to store the number of blocks
 * @nb_unspent: where to store the number of unspent outputs
 *
 * Return: 0 on success, -1 on failure
 */
static int read_header(FILE *fp, int *swap, uint32_t *nb_blocks,
	uint32_t *nb_unspent)
{
	char magic[4], version[3];
	uint8_t endian;

	if (fread(magic, 4, 1, fp) != 1 || memcmp(magic, HBLK_MAGIC, 4) ||
		fread(version, 3, 1, fp) != 1 ||
		memcmp(version, HBLK_VERSION, 3) ||
		fread(&endian, 1, 1, fp) != 1 ||
		fread(nb_blocks, 4, 1, fp) != 1 ||
		fread(nb_unspent, 4, 1, fp) != 1)
		return (-1);
	*swap = (endian != _get_endianness());
	if (*swap)
	{
		SWAPENDIAN(*nb_blocks);
		SWAPENDIAN(*nb_unspent);
	}
	return (0);
}

/**
 * read_tx - reads a single transaction
 *
 * @fp:   open file stream
 * @swap: 1 if multi-byte fields must be byte-swapped
 *
 * Return: pointer to the allocated transaction, or NULL on failure
 */
static transaction_t *read_tx(FILE *fp, int swap)
{
	transaction_t *tx = calloc(1, sizeof(*tx));
	uint32_t nb_in, nb_out, i;
	void *node;

	if (!tx)
		return (NULL);
	tx->inputs = llist_create(MT_SUPPORT_FALSE);
	tx->outputs = llist_create(MT_SUPPORT_FALSE);
	if (!tx->inputs || !tx->outputs ||
		fread(tx->id, SHA256_DIGEST_LENGTH, 1, fp) != 1 ||
		fread(&nb_in, 4, 1, fp) != 1 || fread(&nb_out, 4, 1, fp) != 1)
	{
		transaction_destroy(tx);
		return (NULL);
	}
	if (swap)
	{
		SWAPENDIAN(nb_in);
		SWAPENDIAN(nb_out);
	}
	for (i = 0; i < nb_in + nb_out; i++)
	{
		node = calloc(1, i < nb_in ? sizeof(tx_in_t) : sizeof(tx_out_t));
		if (!node || fread(node, i < nb_in ? TX_IN_SIZE : TX_OUT_SIZE, 1,
			fp) != 1 || llist_add_node(i < nb_in ? tx->inputs :
			tx->outputs, node, ADD_NODE_REAR) == -1)
		{
			free(node);
			transaction_destroy(tx);
			return (NULL);
		}
		if (i >= nb_in && swap)
			SWAPENDIAN(((tx_out_t *)node)->amount);
	}
	return (tx);
}

/**
 * read_block - reads a single Block and its transactions
 *
 * @fp:   open file stream
 * @swap: 1 if multi-byte fields must be byte-swapped
 *
 * Return: pointer to the allocated Block, or NULL on failure
 */
static block_t *read_block(FILE *fp, int swap)
{
	block_t *block = calloc(1, sizeof(*block));
	int32_t nb_tx, i;
	transaction_t *tx;

	if (!block || fread(&block->info, sizeof(block->info), 1, fp) != 1 ||
		fread(&block->data.len, 4, 1, fp) != 1)
		return (free(block), NULL);
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
		fread(block->hash, SHA256_DIGEST_LENGTH, 1, fp) != 1 ||
		fread(&nb_tx, 4, 1, fp) != 1)
		return (free(block), NULL);
	if (swap)
		SWAPENDIAN(nb_tx);
	if (nb_tx == -1)
		return (block);
	block->transactions = llist_create(MT_SUPPORT_FALSE);
	for (i = 0; block->transactions && i < nb_tx; i++)
	{
		tx = read_tx(fp, swap);
		if (!tx || llist_add_node(block->transactions, tx,
			ADD_NODE_REAR) == -1)
			return (transaction_destroy(tx), block_destroy(block), NULL);
	}
	if (!block->transactions)
		return (free(block), NULL);
	return (block);
}

/**
 * read_unspent - reads the list of unspent transaction outputs
 *
 * @fp:   open file stream
 * @swap: 1 if multi-byte fields must be byte-swapped
 * @list: list to fill
 * @nb:   number of unspent outputs to read
 *
 * Return: 0 on success, -1 on failure
 */
static int read_unspent(FILE *fp, int swap, llist_t *list, uint32_t nb)
{
	uint32_t i;
	unspent_tx_out_t *u;

	for (i = 0; i < nb; i++)
	{
		u = calloc(1, sizeof(*u));
		if (!u || fread(u, UNSPENT_SIZE, 1, fp) != 1 ||
			llist_add_node(list, u, ADD_NODE_REAR) == -1)
		{
			free(u);
			return (-1);
		}
		if (swap)
			SWAPENDIAN(u->out.amount);
	}
	return (0);
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
	blockchain_t *bc = NULL;
	block_t *block;
	uint32_t nb_blocks = 0, nb_unspent = 0, i;
	int swap = 0;

	if (!path)
		return (NULL);
	fp = fopen(path, "rb");
	if (!fp)
		return (NULL);
	if (read_header(fp, &swap, &nb_blocks, &nb_unspent) == 0)
		bc = calloc(1, sizeof(*bc));
	if (bc)
	{
		bc->chain = llist_create(MT_SUPPORT_TRUE);
		bc->unspent = llist_create(MT_SUPPORT_TRUE);
	}
	for (i = 0; bc && bc->chain && i < nb_blocks; i++)
	{
		block = read_block(fp, swap);
		if (!block || llist_add_node(bc->chain, block, ADD_NODE_REAR) == -1)
		{
			block_destroy(block);
			blockchain_destroy(bc);
			bc = NULL;
		}
	}
	if (bc && (!bc->chain || !bc->unspent ||
		read_unspent(fp, swap, bc->unspent, nb_unspent) == -1))
	{
		blockchain_destroy(bc);
		bc = NULL;
	}
	fclose(fp);
	return (bc);
}
