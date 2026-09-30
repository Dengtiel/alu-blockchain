#include "../blockchain.h"

void _print_hex_buffer(uint8_t const *buf, size_t len);
int _transaction_print_brief_loop(transaction_t const *transaction,
	unsigned int idx, char const *indent);

/**
 * _print_transactions - prints a Block's list of transactions
 *
 * @transactions: list of transactions (may be NULL)
 */
static void _print_transactions(llist_t *transactions)
{
	int i, size = llist_size(transactions);

	printf("\t\t\ttransactions [%d]: [\n", size);
	for (i = 0; i < size; i++)
		_transaction_print_brief_loop(
			llist_get_node_at(transactions, i), i, "\t\t\t\t");
	printf("\t\t\t]\n");
}

/**
 * _block_print - prints a Block
 *
 * @block: pointer to the Block to print
 */
static void _block_print(block_t const *block)
{
	printf("\t\tBlock: {\n\t\t\tinfo: {\n");
	printf("\t\t\t\tindex: %u,\n", block->info.index);
	printf("\t\t\t\tdifficulty: %u,\n", block->info.difficulty);
	printf("\t\t\t\ttimestamp: %lu,\n",
		(unsigned long)block->info.timestamp);
	printf("\t\t\t\tnonce: %lu,\n", (unsigned long)block->info.nonce);
	printf("\t\t\t\tprev_hash: ");
	_print_hex_buffer(block->info.prev_hash, SHA256_DIGEST_LENGTH);
	printf("\n\t\t\t},\n\t\t\tdata: {\n");
	printf("\t\t\t\tbuffer: \"%.*s\",\n", (int)block->data.len,
		(char const *)block->data.buffer);
	printf("\t\t\t\tlen: %u\n\t\t\t},\n", block->data.len);
	_print_transactions(block->transactions);
	printf("\t\t\thash: ");
	_print_hex_buffer(block->hash, SHA256_DIGEST_LENGTH);
	printf("\n\t\t}\n");
}

/**
 * _blockchain_print - prints an entire Blockchain
 *
 * @blockchain: pointer to the Blockchain to print
 */
void _blockchain_print(blockchain_t const *blockchain)
{
	int i, size;

	if (!blockchain)
		return;
	size = llist_size(blockchain->chain);
	printf("Blockchain: {\n\tchain [%d]: [\n", size);
	for (i = 0; i < size; i++)
		_block_print(llist_get_node_at(blockchain->chain, i));
	printf("\t]\n}\n");
}

/**
 * _block_print_brief - prints a Block in a compact format
 *
 * @block: pointer to the Block to print
 */
static void _block_print_brief(block_t const *block)
{
	printf("\t\tBlock: {\n\t\t\tinfo: { %u, %u, %lu, %lu, ",
		block->info.index, block->info.difficulty,
		(unsigned long)block->info.timestamp,
		(unsigned long)block->info.nonce);
	_print_hex_buffer(block->info.prev_hash, SHA256_DIGEST_LENGTH);
	printf(" },\n\t\t\tdata: { \"%.*s\", %u },\n",
		(int)block->data.len, (char const *)block->data.buffer,
		block->data.len);
	_print_transactions(block->transactions);
	printf("\t\t\thash: ");
	_print_hex_buffer(block->hash, SHA256_DIGEST_LENGTH);
	printf("\n\t\t}\n");
}

/**
 * _blockchain_print_brief - prints an entire Blockchain, compact format
 *
 * @blockchain: pointer to the Blockchain to print
 */
void _blockchain_print_brief(blockchain_t const *blockchain)
{
	int i, size;

	if (!blockchain)
		return;
	size = llist_size(blockchain->chain);
	printf("Blockchain: {\n\tchain [%d]: [\n", size);
	for (i = 0; i < size; i++)
		_block_print_brief(llist_get_node_at(blockchain->chain, i));
	printf("\t]\n}\n");
}
