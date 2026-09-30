#include "../transaction/transaction.h"

void _print_hex_buffer(uint8_t const *buf, size_t len);

/**
 * _transaction_print_brief_loop - prints a transaction in brief format
 *
 * @transaction: transaction to print
 * @idx:         index in the list (unused)
 * @indent:      indentation prefix
 *
 * Return: always 0
 */
int _transaction_print_brief_loop(transaction_t const *transaction,
	unsigned int idx, char const *indent)
{
	tx_out_t const *out;

	(void)idx;
	if (!transaction)
		return (0);
	out = llist_get_head(transaction->outputs);
	printf("%sTransaction: {\n", indent);
	printf("%s\tamount: %u from %d inputs,\n", indent,
		out ? out->amount : 0, llist_size(transaction->inputs));
	printf("%s\treceiver: ", indent);
	if (out)
		_print_hex_buffer(out->pub, EC_PUB_LEN);
	printf("\n%s\tid: ", indent);
	_print_hex_buffer(transaction->id, SHA256_DIGEST_LENGTH);
	printf("\n%s}\n", indent);
	return (0);
}

/**
 * _transaction_print_brief - prints a transaction in brief format
 *
 * @transaction: transaction to print
 */
void _transaction_print_brief(transaction_t const *transaction)
{
	_transaction_print_brief_loop(transaction, 0, "");
}
