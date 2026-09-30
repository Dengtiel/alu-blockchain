#include "../blockchain.h"

block_t const _genesis = {
	{ 0, 0, GENESIS_TIMESTAMP, 0, { 0 } },
	{ GENESIS_DATA, GENESIS_DATA_LEN },
	GENESIS_HASH
};
