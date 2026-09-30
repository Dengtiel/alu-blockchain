#include "explorer.h"

/**
 * html_unspent - writes the table of unspent transaction outputs
 *
 * @fp:      output stream
 * @unspent: list of unspent transaction outputs
 */
void html_unspent(FILE *fp, llist_t *unspent)
{
	int i, size = llist_size(unspent);
	unspent_tx_out_t *u;

	fprintf(fp, "<h2>Unspent outputs (%d)</h2>\n", size > 0 ? size : 0);
	if (size <= 0)
	{
		fprintf(fp, "<p class=\"muted\">No unspent outputs.</p>\n");
		return;
	}
	fprintf(fp, "<div class=\"scroll\"><table>\n<tr><th>Amount</th>");
	fprintf(fp, "<th>Owner</th><th>Transaction</th><th>Block</th></tr>\n");
	for (i = 0; i < size; i++)
	{
		u = llist_get_node_at(unspent, i);
		fprintf(fp, "<tr><td><b>%u</b></td><td>", u->out.amount);
		html_short_hex(fp, u->out.pub, EC_PUB_LEN);
		fprintf(fp, "</td><td>");
		html_short_hex(fp, u->tx_id, SHA256_DIGEST_LENGTH);
		fprintf(fp, "</td><td>");
		html_short_hex(fp, u->block_hash, SHA256_DIGEST_LENGTH);
		fprintf(fp, "</td></tr>\n");
	}
	fprintf(fp, "</table></div>\n");
}
