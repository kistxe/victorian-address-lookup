# Victorian Address Lookup in C

This project compares two data structures for address lookup:
- Linked List
- Patricia Tree (radix trie)
The dataset consists of Victorian addresses used to test search performance.

## Stage 1 - Linked List

The linked list performs lookups by sequentially comparing each key.
Time Complexity:  
O(n)

## Stage 2 - Patricia Tree

A Patricia tree stores keys using shared prefixes, enabling faster lookup and prefix matching.
Time Complexity:  
O(log n)

## Hypothesis

For larger datasets, the Patricia tree is expected to outperform the linked list due to prefix compression and faster search.

## Dataset

Victorian address dataset (first 1067 entries included in sample dataset).

## Build

Compile the program using: make dict2

## Run

Example execution:

./dict2 2 data/dataset_1067.csv output.txt < tests/sample_query.in

The program reads queries from standard input and performs address lookup using a Patricia tree.

Arguments:
- '2': Stage identifier (Stage 1 for linked list, Stage 2 for Patricia tree)
- 'data/dataset_1067.csv': Address dataset
- 'output.txt': Program output
- '< tests/sample_query.in': Input queries

Results are written to: output.txt
