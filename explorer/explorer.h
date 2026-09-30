#ifndef _EXPLORER_H_
#define _EXPLORER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "blockchain.h"

void html_hex(FILE *fp, uint8_t const *buf, size_t len);
void html_short_hex(FILE *fp, uint8_t const *buf, size_t len);
void html_text(FILE *fp, int8_t const *buf, size_t len);
void html_head(FILE *fp, char const *source);
void html_foot(FILE *fp);
void html_summary(FILE *fp, blockchain_t const *bc, int nb_valid);
void html_block(FILE *fp, block_t const *block, int valid);
void html_unspent(FILE *fp, llist_t *unspent);
int *chain_audit(blockchain_t const *bc, int *nb_valid);

#endif /* _EXPLORER_H_ */
