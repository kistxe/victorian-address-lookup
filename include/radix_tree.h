#ifndef RADIX_TREE_H
#define RADIX_TREE_H

#include <stddef.h>
#include "records.h"
#include "dictionary.h"

typedef struct RadixNode {
	char *stem; // bit array (not null-terminated text)
	unsigned stem_bits; // number of bits stored in stem
	struct RadixNode *child[2]; // next bit: 0 or 1

	AddressRecord **items;
	size_t count;
	size_t cap;
} RadixNode;

typedef struct {
	RadixNode *root;
} RadixTree;

void rt_init(RadixTree *t);
void rt_free(RadixTree *t);

int rt_insert(RadixTree *t, AddressRecord *rec);

int rt_search_best(RadixTree *t, const char *query, SearchResults *out,
	CompareCounts *counts);

#endif
