#include "hblk_crypto.h"

/**
 * ec_verify - Verifies the signature of a given set of bytes
 * @key: Pointer to the EC_KEY structure containing the public key
 * @msg: Pointer to the bytes whose signature is to be verified
 * @msglen: Number of bytes in @msg
 * @sig: Pointer to the signature to be checked
 *
 * Return: 1 if the signature is valid, 0 otherwise
 */
int ec_verify(EC_KEY const *key, uint8_t const *msg, size_t msglen,
	sig_t const *sig)
{
	int result;

	if (key == NULL || msg == NULL || sig == NULL)
		return (0);

	result = ECDSA_verify(0, msg, msglen, sig->sig, sig->len,
		(EC_KEY *)key);

	if (result != 1)
		return (0);

	return (1);
}
