# Victorian Address Lookup in C

This project compares two data structures for address lookup:

- Linked List
- Patricia Tree (radix trie)

The dataset consists of Victorian addresses used to test search performance.

## Stage 1 — Linked List

The linked list performs lookups by sequentially comparing each key.

Time Complexity:  
O(n)

## Stage 2 — Patricia Tree

A Patricia tree stores keys using shared prefixes, enabling faster lookup and prefix matching.

Time Complexity:  
O(log n)

## Hypothesis

For larger datasets, the Patricia tree is expected to outperform the linked list due to prefix compression and faster search.

## Dataset

Victorian address dataset (first 1067 entries included in sample dataset).