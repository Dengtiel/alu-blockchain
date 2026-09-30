#include "blockchain.h"

/**
 * hash_matches_difficulty - checks whether a hash matches a difficulty
 *
 * @hash:       hash to check
 * @difficulty: minimum number of leading zero bits
 *
 * Return: 1 if the difficulty is respected, 0 otherwise
 */
int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
	uint32_t difficulty)
{
	uint32_t zeros = 0;
	size_t i;
	int bit;

	if (!hash)
		return (0);
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		if (hash[i] == 0)
		{
			zeros += 8;
			continue;
		}
		for (bit = 7; bit >= 0 && !((hash[i] >> bit) & 1); bit--)
			zeros++;
		break;
	}
	return (zeros >= difficulty);
}
