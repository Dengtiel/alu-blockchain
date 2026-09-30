# hblk_explorer — Visual Blockchain Explorer

Loads a serialized Blockchain (`.hblk`, format 0.3), re-audits every Block,
and generates a self-contained HTML page.

## Build

    make

Requires `libhblk_blockchain.a` (blockchain/v0.3) and `libhblk_crypto.a` (crypto).

## Usage

    ./hblk_explorer <blockchain.hblk> [output.html]

Default output: `explorer.html`. Try it with the included sample:

    ./hblk_explorer sample.hblk sample.html

## What the page shows

- Summary: blocks, valid blocks, transactions, unspent outputs, coins in circulation
- Each Block: hash, link to the previous Block, time, difficulty, nonce, data,
  and a valid/invalid badge
- Each transaction: coinbase reward or signed inputs, outputs with amount and receiver
- The table of unspent transaction outputs
- Hover any shortened hash to see it in full

## Integrity audit

The chain is replayed from the Genesis Block: each Block is checked with
`block_is_valid` against the unspent outputs rebuilt from the previous Blocks,
then `update_unspent` is applied. A modified file is detected, and the page
shows exactly which Blocks are broken.

## Workflow with the CLI

    cd ../cli && ./cli
    blockchain> mine
    blockchain> send 10 <address>
    blockchain> mine
    blockchain> save ../explorer/my_chain.hblk
    blockchain> exit
    cd ../explorer && ./hblk_explorer my_chain.hblk
