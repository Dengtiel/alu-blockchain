#include "cli.h"

static command_t const commands[] = {
	{"wallet_load", cmd_wallet_load, "wallet_load <path>  Load wallet"},
	{"wallet_save", cmd_wallet_save, "wallet_save <path>  Save wallet"},
	{"send", cmd_send, "send <amount> <address>  Send coins"},
	{"mine", cmd_mine, "mine  Mine a Block"},
	{"info", cmd_info, "info  Display Blockchain information"},
	{"load", cmd_load, "load <path>  Load a Blockchain from a file"},
	{"save", cmd_save, "save <path>  Save the Blockchain into a file"},
	{"help", cmd_help, "help  List commands"},
	{NULL, NULL, NULL}
};

/**
 * cmd_help - lists the available commands
 *
 * @state: CLI state (unused)
 * @arg1:  unused
 * @arg2:  unused
 *
 * Return: always 0
 */
int cmd_help(state_t *state, char *arg1, char *arg2)
{
	size_t i;

	(void)state;
	(void)arg1;
	(void)arg2;
	printf("Commands:\n");
	for (i = 0; commands[i].name; i++)
		printf("  %s\n", commands[i].usage);
	printf("  exit  Quit the CLI\n");
	return (0);
}

/**
 * state_init - loads or creates the wallet, creates Blockchain and pool
 *
 * @state:       CLI state to initialize
 * @wallet_path: folder to load the wallet from
 *
 * Return: 0 on success, -1 on failure
 */
static int state_init(state_t *state, char const *wallet_path)
{
	memset(state, 0, sizeof(*state));
	state->wallet = ec_load(wallet_path);
	if (state->wallet)
		printf("Wallet loaded from '%s'\n", wallet_path);
	else
	{
		state->wallet = ec_create();
		if (!state->wallet)
			return (-1);
		printf("New wallet created (use 'wallet_save <path>' to keep it)\n");
	}
	state->blockchain = blockchain_create();
	state->tx_pool = llist_create(MT_SUPPORT_FALSE);
	if (!state->blockchain || !state->tx_pool)
		return (-1);
	return (0);
}

/**
 * state_free - frees everything held by the CLI state
 *
 * @state: CLI state to free
 */
static void state_free(state_t *state)
{
	if (state->tx_pool)
		llist_destroy(state->tx_pool, 1, (node_dtor_t)transaction_destroy);
	blockchain_destroy(state->blockchain);
	if (state->wallet)
		EC_KEY_free(state->wallet);
}

/**
 * dispatch - parses a command line and runs the matching command
 *
 * @state: CLI state
 * @line:  command line (modified)
 *
 * Return: 1 if the CLI must exit, 0 otherwise
 */
static int dispatch(state_t *state, char *line)
{
	char *cmd, *arg1, *arg2;
	size_t i;

	cmd = strtok(line, " \t\n");
	if (!cmd)
		return (0);
	if (!strcmp(cmd, "exit") || !strcmp(cmd, "quit"))
		return (1);
	arg1 = strtok(NULL, " \t\n");
	arg2 = strtok(NULL, " \t\n");
	for (i = 0; commands[i].name; i++)
	{
		if (!strcmp(cmd, commands[i].name))
		{
			commands[i].func(state, arg1, arg2);
			return (0);
		}
	}
	printf("Unknown command '%s'. Type 'help' for the list.\n", cmd);
	return (0);
}

/**
 * main - Entry point of the Blockchain CLI
 *
 * @argc: argument count
 * @argv: argument vector (optional wallet folder in argv[1])
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char **argv)
{
	state_t state;
	char *line = NULL;
	size_t size = 0;
	int done = 0;

	if (state_init(&state, argc > 1 ? argv[1] : DEFAULT_WALLET) == -1)
	{
		fprintf(stderr, "Failed to initialize the CLI\n");
		state_free(&state);
		return (EXIT_FAILURE);
	}
	printf("Type 'help' to list the commands\n");
	while (!done)
	{
		printf("%s", CLI_PROMPT);
		fflush(stdout);
		if (getline(&line, &size, stdin) == -1)
		{
			printf("\n");
			break;
		}
		done = dispatch(&state, line);
	}
	free(line);
	state_free(&state);
	return (EXIT_SUCCESS);
}
