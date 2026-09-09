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

#endif /* HBLK_CRYPTO_H */
