#include "transaction.h"

/**
 * tx_in_sign - signs a transaction input
 *
 * @in:          transaction input to sign
 * @tx_id:       ID of the transaction the input is stored in
 * @sender:      private key of the receiver of the referenced output
 * @all_unspent: list of all unspent transaction outputs to date
 *
 * Return: pointer to the resulting signature, or NULL on failure
 */
sig_t *tx_in_sign(tx_in_t *in, uint8_t const tx_id[SHA256_DIGEST_LENGTH],
	EC_KEY const *sender, llist_t *all_unspent)
{
	unspent_tx_out_t *unspent = NULL;
	uint8_t pub[EC_PUB_LEN];
	int i, size;

	if (!in || !tx_id || !sender || !all_unspent)
		return (NULL);
	size = llist_size(all_unspent);
	for (i = 0; i < size; i++)
	{
		unspent = llist_get_node_at(all_unspent, i);
		if (!memcmp(unspent->out.hash, in->tx_out_hash,
			SHA256_DIGEST_LENGTH))
			break;
	}
	if (i >= size)
		return (NULL);
	if (!ec_to_pub(sender, pub) ||
		memcmp(pub, unspent->out.pub, EC_PUB_LEN))
		return (NULL);
	if (!ec_sign(sender, tx_id, SHA256_DIGEST_LENGTH, &in->sig))
		return (NULL);
	return (&in->sig);
}
