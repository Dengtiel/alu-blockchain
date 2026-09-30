# holbertonschool-blockchain

Blockchain project — cryptography utilities (SHA256, EC key generation/signing) built with OpenSSL, and a C blockchain implementation.

## Crypto module

Located in `crypto/`. Provides:
- `sha256`: SHA256 hashing of a byte sequence

## blockchain/v0.3 — Transactions

- Transaction outputs, inputs and unspent outputs (UTXO model)
- Transaction creation, ECDSA signing and validation
- Coinbase transactions (block reward of 50 coins)
- Block mining with proof of work (difficulty in leading zero bits)
- Serialization format HBLK 0.3 including transactions and unspent outputs
- Static library: `make libhblk_blockchain.a`
