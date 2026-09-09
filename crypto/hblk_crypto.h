#ifndef HBLK_CRYPTO_H
#define HBLK_CRYPTO_H

#define OPENSSL_API_COMPAT 0x10100000L
#define OPENSSL_NO_DEPRECATED

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

/* Elliptic curve used for key generation */
#define EC_CURVE NID_secp256k1

/* Length (in bytes) of an uncompressed EC public key: 1 + 32 + 32 */
#define EC_PUB_LEN 65

/* Filenames used to store EC key pairs on disk */
#define PRI_FILENAME "key.pem"
#define PUB_FILENAME "key_pub.pem"

/* Maximum length (in bytes) of an ECDSA signature on secp256k1 */
#define SIG_MAX_LEN 72

/**
 * struct sig_s - Signature structure
 * @sig: Buffer containing the actual signature
 * @len: Length of the signature
 */
typedef struct sig_s
{
	uint8_t sig[SIG_MAX_LEN];
	size_t  len;
} sig_t;

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

/**
 * ec_save - Saves an EC key pair to disk in PEM format
 * @key: Pointer to the EC key pair to save
 * @folder: Path to the folder in which to save the keys
 *
 * Return: 1 upon success, or 0 upon failure
 */
int ec_save(EC_KEY *key, char const *folder);

/**
 * ec_load - Loads an EC key pair from disk
 * @folder: Path to the folder from which to load the keys
 *
 * Return: Pointer to the created EC key pair upon success,
 *         or NULL upon failure
 */
EC_KEY *ec_load(char const *folder);

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
	sig_t *sig);

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
	sig_t const *sig);

#endif /* HBLK_CRYPTO_H */
