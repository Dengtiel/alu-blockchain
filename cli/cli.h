#ifndef _CLI_H_
#define _CLI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockchain.h"

#define CLI_PROMPT "blockchain> "
#define DEFAULT_WALLET "wallet"

/**
 * struct state_s - CLI state
 *
 * @wallet:     current wallet (EC key pair)
 * @blockchain: local Blockchain
 * @tx_pool:    local transaction pool (list of transaction_t *)
 */
typedef struct state_s
{
	EC_KEY *wallet;
	blockchain_t *blockchain;
	llist_t *tx_pool;
} state_t;

/**
 * struct command_s - CLI command descriptor
 *
 * @name:  command name
 * @func:  command handler
 * @usage: usage string
 */
typedef struct command_s
{
	char const *name;
	int (*func)(state_t *state, char *arg1, char *arg2);
	char const *usage;
} command_t;

void print_hex(uint8_t const *buf, size_t len);

int cmd_help(state_t *state, char *arg1, char *arg2);
int cmd_wallet_load(state_t *state, char *path, char *arg2);
int cmd_wallet_save(state_t *state, char *path, char *arg2);
int cmd_send(state_t *state, char *amount_str, char *address);
int cmd_mine(state_t *state, char *arg1, char *arg2);
int cmd_info(state_t *state, char *arg1, char *arg2);
int cmd_load(state_t *state, char *path, char *arg2);
int cmd_save(state_t *state, char *path, char *arg2);

#endif /* _CLI_H_ */
