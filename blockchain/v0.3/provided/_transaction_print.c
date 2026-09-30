#include "../transaction/transaction.h"

void _print_hex_buffer(uint8_t const *buf, size_t len);

/**
 * _tx_in_print - prints a transaction input
 *
 * @in: transaction input to print
 */
static void _tx_in_print(tx_in_t const *in)
{
	printf("\t\t{\n\t\t\tblock_hash: ");
	_print_hex_buffer(in->block_hash, SHA256_DIGEST_LENGTH);
	printf(",\n\t\t\ttx_id: ");
	_print_hex_buffer(in->tx_id, SHA256_DIGEST_LENGTH);
	printf(",\n\t\t\ttx_out_hash: ");
	_print_hex_buffer(in->tx_out_hash, SHA256_DIGEST_LENGTH);
	printf(",\n\t\t\tsig: ");
	if (in->sig.len)
		_print_hex_buffer(in->sig.sig, in->sig.len);
	else
		printf("null");
	printf("\n\t\t}\n");
}

/**
 * _tx_out_print - prints a transaction output
 *
 * @out: transaction output to print
 */
static void _tx_out_print(tx_out_t const *out)
{
	printf("\t\t{\n\t\t\tamount: %u,\n\t\t\tpub: ", out->amount);
	_print_hex_buffer(out->pub, EC_PUB_LEN);
	printf(",\n\t\t\thash: ");
	_print_hex_buffer(out->hash, SHA256_DIGEST_LENGTH);
	printf("\n\t\t}\n");
}

/**
 * _transaction_print - prints a transaction
 *
 * @transaction: transaction to print
 */
void _transaction_print(transaction_t const *transaction)
{
	int i, size;

	if (!transaction)
		return;
	printf("Transaction: {\n");
	size = llist_size(transaction->inputs);
	printf("\tinputs [%d]: [\n", size);
	for (i = 0; i < size; i++)
		_tx_in_print(llist_get_node_at(transaction->inputs, i));
	size = llist_size(transaction->outputs);
	printf("\t],\n\toutputs [%d]: [\n", size);
	for (i = 0; i < size; i++)
		_tx_out_print(llist_get_node_at(transaction->outputs, i));
	printf("\t],\n\tid: ");
	_print_hex_buffer(transaction->id, SHA256_DIGEST_LENGTH);
	printf("\n}\n");
}
