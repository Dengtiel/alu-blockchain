#include "explorer.h"

/**
 * write_page - writes the whole explorer page
 *
 * @fp:       output stream
 * @bc:       Blockchain
 * @status:   per-Block audit status
 * @nb_valid: number of valid Blocks
 * @source:   path of the loaded Blockchain file
 */
static void write_page(FILE *fp, blockchain_t const *bc, int const *status,
	int nb_valid, char const *source)
{
	int i, size = llist_size(bc->chain);

	html_head(fp, source);
	html_summary(fp, bc, nb_valid);
	for (i = 0; i < size; i++)
	{
		if (i)
			fprintf(fp, "<div class=\"link\">&darr; linked by hash</div>\n");
		html_block(fp, llist_get_node_at(bc->chain, i), status[i]);
	}
	html_unspent(fp, bc->unspent);
	html_foot(fp);
}

/**
 * main - Entry point of the Blockchain explorer
 *
 * @argc: argument count
 * @argv: argv[1] is the .hblk file, argv[2] the optional HTML output
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char **argv)
{
	blockchain_t *bc;
	FILE *fp = NULL;
	int *status, nb_valid = 0;
	char const *out = argc > 2 ? argv[2] : "explorer.html";

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <blockchain.hblk> [output.html]\n",
			argv[0]);
		return (EXIT_FAILURE);
	}
	bc = blockchain_deserialize(argv[1]);
	if (!bc)
	{
		fprintf(stderr, "Cannot load a Blockchain from '%s'\n", argv[1]);
		return (EXIT_FAILURE);
	}
	status = chain_audit(bc, &nb_valid);
	if (status)
		fp = fopen(out, "w");
	if (!fp)
	{
		fprintf(stderr, "Cannot write '%s'\n", out);
		free(status);
		blockchain_destroy(bc);
		return (EXIT_FAILURE);
	}
	write_page(fp, bc, status, nb_valid, argv[1]);
	fclose(fp);
	printf("Explorer written to '%s': %d blocks, %d valid\n", out,
		llist_size(bc->chain), nb_valid);
	free(status);
	blockchain_destroy(bc);
	return (EXIT_SUCCESS);
}
