#ifndef HBLK_CRYPTO_H
#define HBLK_CRYPTO_H

#define OPENSSL_API_COMPAT 0x10100000L
#define OPENSSL_NO_DEPRECATED

#include <stdint.h>
#include <stddef.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>

/* Elliptic curve used for key generation */
#define EC_CURVE NID_secp256k1

/* Length (in bytes) of an uncompressed EC public key: 1 + 32 + 32 */
#define EC_PUB_LEN 65

/**
 * sha256 - Computes the SHA256 hash of a sequence of bytes
 * @s: Sequence of bytes to be hashed
 * @len: Number of bytes to hash in @s
 * @digest: Buffer in which to store the resulting hash
 *
 * Return: A pointer to @digest, or NULL if @digest is NULL
 */
uint8_t *sha256(int8_t const *s, size_t len,
	uint8_t digest[SHA256_DIGEST_LENGTH]);

/**
 * ec_create - Creates a new EC key pair
 *
 * Return: Pointer to an EC_KEY structure containing both the
 *         public and private keys, or NULL upon failure
 */
EC_KEY *ec_create(void);

/**
 * ec_to_pub - Extracts the public key from an EC_KEY structure
 * @key: Pointer to the EC_KEY structure to retrieve the public key from
 * @pub: Buffer in which to store the extracted public key (uncompressed)
 *
 * Return: A pointer to @pub, or NULL upon failure
 */
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN]);

/**
 * ec_from_pub - Creates an EC_KEY structure given a public key
 * @pub: Buffer containing the public key to be converted
 *
 * Return: Pointer to the created EC_KEY structure upon success,
 *         or NULL upon failure
 */
EC_KEY *ec_from_pub(uint8_t const pub[EC_PUB_LEN]);

#endif /* HBLK_CRYPTO_H */
