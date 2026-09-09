#include "hblk_crypto.h"

/**
 * ec_sign - Signs a given set of bytes using an EC private key
 * @key: Pointer to the EC_KEY structure containing the private key
 * @msg: Pointer to the bytes to be signed
 * @msglen: Number of bytes to sign in @msg
 * @sig: Buffer in which to store the signature
 *
 * Return: A pointer to the signature buffer upon success (sig->sig),
 *         or NULL upon failure
 */
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen,
	sig_t *sig)
{
	unsigned int siglen;

	if (key == NULL || msg == NULL || sig == NULL)
		return (NULL);

	if (ECDSA_sign(0, msg, msglen, sig->sig, &siglen,
		(EC_KEY *)key) != 1)
		return (NULL);

	sig->len = siglen;

	return (sig->sig);
}
