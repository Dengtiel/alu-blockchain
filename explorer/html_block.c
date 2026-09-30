#include "explorer.h"

/**
 * html_tx_in - writes a transaction input
 *
 * @fp: output stream
 * @in: transaction input
 */
static void html_tx_in(FILE *fp, tx_in_t const *in)
{
	fprintf(fp, "<p>Spends ");
	html_short_hex(fp, in->tx_out_hash, SHA256_DIGEST_LENGTH);
	fprintf(fp, " from tx ");
	html_short_hex(fp, in->tx_id, SHA256_DIGEST_LENGTH);
	fprintf(fp, " %s</p>\n",
		in->sig.len ? "<span class=\"tag\">signed</span>" : "");
}

/**
 * html_tx_out - writes a transaction output
 *
 * @fp:  output stream
 * @out: transaction output
 */
static void html_tx_out(FILE *fp, tx_out_t const *out)
{
	fprintf(fp, "<p><b>%u</b> coins &rarr; ", out->amount);
	html_short_hex(fp, out->pub, EC_PUB_LEN);
	fprintf(fp, "</p>\n");
}

/**
 * html_tx - writes a transaction with its inputs and outputs
 *
 * @fp:       output stream
 * @tx:       transaction
 * @coinbase: 1 if this is the coinbase transaction
 */
static void html_tx(FILE *fp, transaction_t const *tx, int coinbase)
{
	int i, n;

	fprintf(fp, "<div class=\"tx\"><div>%sTransaction ",
		coinbase ? "<span class=\"tag\">coinbase</span> " : "");
	html_short_hex(fp, tx->id, SHA256_DIGEST_LENGTH);
	fprintf(fp, "</div>\n<div class=\"io\"><div><h4>Inputs</h4>\n");
	if (coinbase)
		fprintf(fp, "<p class=\"muted\">Block reward (new coins)</p>\n");
	n = llist_size(tx->inputs);
	for (i = 0; !coinbase && i < n; i++)
		html_tx_in(fp, llist_get_node_at(tx->inputs, i));
	fprintf(fp, "</div><div><h4>Outputs</h4>\n");
	n = llist_size(tx->outputs);
	for (i = 0; i < n; i++)
		html_tx_out(fp, llist_get_node_at(tx->outputs, i));
	fprintf(fp, "</div></div></div>\n");
}

/**
 * html_block_info - writes the header fields of a Block
 *
 * @fp:    output stream
 * @block: Block
 */
static void html_block_info(FILE *fp, block_t const *block)
{
	char date[32];
	time_t t = (time_t)block->info.timestamp;
	struct tm *tm = gmtime(&t);

	if (!tm || !strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S UTC", tm))
		strcpy(date, "unknown");
	fprintf(fp, "<dl>\n<dt>Hash</dt><dd class=\"mono\">");
	html_hex(fp, block->hash, SHA256_DIGEST_LENGTH);
	fprintf(fp, "</dd>\n<dt>Previous</dt><dd class=\"mono\">");
	if (block->info.index)
		fprintf(fp, "<a href=\"#block-%u\">", block->info.index - 1);
	html_hex(fp, block->info.prev_hash, SHA256_DIGEST_LENGTH);
	fprintf(fp, "%s</dd>\n", block->info.index ? "</a>" : "");
	fprintf(fp, "<dt>Mined</dt><dd>%s</dd>\n", date);
	fprintf(fp, "<dt>Difficulty</dt><dd>%u</dd>\n", block->info.difficulty);
	fprintf(fp, "<dt>Nonce</dt><dd>%lu</dd>\n",
		(unsigned long)block->info.nonce);
	fprintf(fp, "<dt>Data</dt><dd>");
	html_text(fp, block->data.buffer, block->data.len);
	fprintf(fp, "</dd>\n</dl>\n");
}

/**
 * html_block - writes a Block card with its transactions
 *
 * @fp:    output stream
 * @block: Block
 * @valid: 1 if the Block passed the audit, 0 otherwise
 */
void html_block(FILE *fp, block_t const *block, int valid)
{
	int i, nb_tx = llist_size(block->transactions);

	fprintf(fp, "<section class=\"block%s\" id=\"block-%u\">\n",
		valid ? "" : " bad", block->info.index);
	fprintf(fp, "<h2>Block #%u <span class=\"badge\">%s</span></h2>\n",
		block->info.index, valid ? "valid" : "invalid");
	html_block_info(fp, block);
	fprintf(fp, "<h3>Transactions (%d)</h3>\n", nb_tx > 0 ? nb_tx : 0);
	for (i = 0; i < nb_tx; i++)
		html_tx(fp, llist_get_node_at(block->transactions, i), i == 0);
	fprintf(fp, "</section>\n");
}
