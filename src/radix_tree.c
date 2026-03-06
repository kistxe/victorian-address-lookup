#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "radix_tree.h"
#include "bit.h"
#include "records.h"

// Dynamic array of records
void append_record(RadixNode *n, AddressRecord *rec) {
	if (n->count == n->cap) {
		n->cap = n->cap ? n->cap * 2 : 4;
		n->items = realloc(n->items, n->cap * sizeof(*n->items));
		assert(n->items);
	}
	n->items[n->count++] = rec;
}

// Node helpers
static RadixNode *new_node(char *stem, unsigned stem_bits) {
	RadixNode *n = calloc(1, sizeof *n);
	assert(n);
	n->stem = stem;
	n->stem_bits = stem_bits;
	return n;
}

void node_free(RadixNode *n) {
	if (!n) return;
	free(n->stem);
	free(n->items);
	node_free(n->child[0]);
	node_free(n->child[1]);
	free(n);
}

void rt_init(RadixTree *t){ t->root = NULL; }
void rt_free(RadixTree *t){ node_free(t->root); t->root = NULL; }

// Longest common prefix in bits between node stem and query bits at offset.
// Increments *bits_counted for each compared bit
unsigned longest_common_prefix(const char *key, unsigned key_off_bits, 
	const char *stem, unsigned stem_bits, unsigned long *bits_counted) {

	unsigned i = 0;
	for(i = 0; i < stem_bits; i++) {
		int kb = getBit((char*)key, key_off_bits + i);
		int sb = getBit((char*)stem, i);
		if (bits_counted) {
			(*bits_counted)++;
		}
		if (kb != sb) {
			break;
		}
	}
	return i;
}

// Inserts new address record into radix tree
int rt_insert(RadixTree *t, AddressRecord *rec) {
	const char *key = record_get_key(rec);
	unsigned key_bits = ((unsigned)strlen(key) + 1U) * BITS_PER_BYTE;
	
	if (!t->root) {
		t->root = new_node(createStem((char*)key, 0, key_bits), key_bits);
		append_record(t->root, rec);
		return 0;
	}

	RadixNode *curr_node = t->root;
	unsigned bit_offset = 0;

	while (1) {
		unsigned long dummy_bits = 0;
		unsigned lcp = longest_common_prefix(key, bit_offset, curr_node->stem, 
			curr_node->stem_bits, &dummy_bits);

		if (lcp < curr_node->stem_bits) {
			// Split current node at longest common prefix
			unsigned old_tail_bits = curr_node->stem_bits - lcp;

			// Old child from remaining old stem, move children down
			char *old_stem = createStem(curr_node->stem, lcp, old_tail_bits);
			RadixNode *old = new_node(old_stem, old_tail_bits);
			old->child[0] = curr_node->child[0];
			old->child[1] = curr_node->child[1];
			old->items = curr_node->items;
			old->count = curr_node->count;
			old->cap = curr_node->cap;

			// Parent becomes common prefix
			char *new_parent_stem = createStem(curr_node->stem, 0, lcp);
			free(curr_node->stem);
			curr_node->stem = new_parent_stem;
			curr_node->stem_bits = lcp;
			curr_node->child[0] = curr_node->child[1] = NULL;
			curr_node->items = NULL; 
			curr_node->count = 0;
			curr_node->cap = 0;

			int old_first_bit = getBit(old->stem, 0);
			curr_node->child[old_first_bit] = old;

			// New child for remaining new key bits
			unsigned new_tail_bits = key_bits - (bit_offset + lcp);
			char *new_stem = createStem((char*)key, bit_offset + lcp, new_tail_bits);
			RadixNode *nn = new_node(new_stem, new_tail_bits);
			append_record(nn, rec);

			int new_first_bit;
			if (new_tail_bits) {
				new_first_bit = getBit(nn->stem, 0);
			}
			else {
				new_first_bit = 0;
			}
			curr_node->child[new_first_bit] = nn;
			return 0;
		}

		// Full node stem matched
		bit_offset += lcp;
		if (bit_offset == key_bits) {
			// Terminal of key at this node
			append_record(curr_node, rec);
			return 0;
		}

		int bit = getBit((char*)key, bit_offset);
		if (!curr_node->child[bit]) {
			unsigned tail_bits = key_bits - bit_offset;
			RadixNode *nn = new_node(createStem((char*)key, bit_offset, tail_bits), tail_bits);
			append_record(nn, rec);
			curr_node->child[bit] = nn;
			return 0;
		}
		curr_node = curr_node->child[bit];
	}
}

// Upon mismatch, becomes candidate key with possible match
typedef struct {
	const char *key; // Candidate EZI_ADD
	AddressRecord **items; // Records at candidate node
	size_t count;
} Candidate;

typedef struct {
	Candidate *first_element; // Pointer to the first element in the array
	size_t n; 
	size_t cap; // Total capacity
} CandVec;

// Adds new candidate to array 
void cand_push(CandVec *v, const char *key, AddressRecord **items, size_t count) {
	if (v->n == v->cap) {
		v->cap = v->cap ? v->cap * 2 : 8;
		v->first_element = realloc(v->first_element, v->cap*sizeof *v->first_element); 
		assert(v->first_element);
	}
	v->first_element[v->n++] = (Candidate){ key, items, count };
}

void collect_candidates(RadixNode *n, unsigned long *node_count, CandVec *out) {
	if (!n) return;
	
	// If 'n' store records, add a candidate
	if (n->count > 0) {
		const char *k = record_get_key(n->items[0]); // identical across records
		cand_push(out, k, n->items, n->count);
	}

	// Visit child[0]
	if (n->child[0]) {
		if (node_count) {
			(*node_count)++; // node access count
		}
		collect_candidates(n->child[0], node_count, out);
	}

	// Visit child[1]
	if (n->child[1]) {
		if (node_count) {
			(*node_count)++;
		}
		collect_candidates(n->child[1], node_count, out);
	}
}

// Edit distance
/* Returns min of 3 integers 
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int min(int a, int b, int c) {
    if (a < b) {
        if(a < c) {
            return a;
        } else {
            return c;
        }
    } else {
        if(b < c) {
            return b;
        } else {
            return c;
        }
    }
}

/* Returns the edit distance of two strings
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int editDistance(char *str1, char *str2, int n, int m){
    assert(m >= 0 && n >= 0 && (str1 || m == 0) && (str2 || n == 0));
    // Declare a 2D array to store the dynamic programming
    // table
    int dp[n + 1][m + 1];

    // Initialize the dp table
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            // If the first string is empty, the only option
            // is to insert all characters of the second
            // string
            if (i == 0) {
                dp[i][j] = j;
            }
            // If the second string is empty, the only
            // option is to remove all characters of the
            // first string
            else if (j == 0) {
                dp[i][j] = i;
            }
            // If the last characters are the same, no
            // modification is necessary to the string.
            else if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = min(1 + dp[i - 1][j], 1 + dp[i][j - 1],
                    dp[i - 1][j - 1]);
            }
            // If the last characters are different,
            // consider all three operations and find the
            // minimum
            else {
                dp[i][j] = 1 + min(dp[i - 1][j], dp[i][j - 1],
                    dp[i - 1][j - 1]);
            }
        }
    }

    // Return the result from the dynamic programming table
    return dp[n][m];
}


// Search for exact match and on mismatch, compute best candidate by edit distance.
// Fills SearchResults with all records of the chosen key.
int rt_search_best(RadixTree *t, const char *query, SearchResults *out,
	CompareCounts *counts) {
	out->count = 0;
	out->items = NULL;
	
	if (!t->root) {
		return 0;
	}

	unsigned query_bits = ((unsigned)strlen(query) + 1U) * BITS_PER_BYTE;
	RadixNode *curr_node = t->root;
	unsigned bit_offset = 0;

	// Descent with counting
	while (curr_node) {
		counts->nodes++;
		unsigned lcp = longest_common_prefix(query, bit_offset, curr_node->stem,
			curr_node->stem_bits, &counts->bits);
		
		if (lcp < curr_node->stem_bits) {
			// Mismatch inside current node
			break;
		}

		bit_offset += lcp;
		if (bit_offset == query_bits) {
			// Exact terminal header
			if (curr_node->count > 0) {
				counts->strings += 1;
				out->count = curr_node->count;
				out->items = malloc(out->count * sizeof *out->items);
				assert(out->items);
				for (size_t i = 0; i < out->count; i++) {
					out->items[i] = curr_node->items[i];
				}
			}
			return 0;
		}
		int bit = getBit((char*)query, bit_offset);
		if (!curr_node->child[bit]) {
			// Missing child mismatch
			break;
		}
		curr_node = curr_node->child[bit];
	}

	// Mismatch where candidates are equal to all descendants with records
	CandVec C = {0};

	collect_candidates(curr_node, &counts->nodes, &C);
	if (C.n == 0) {
		free(C.first_element);
		return 0; // NOTFOUND
	}

	// Choose minimal edit distance
	int best_distance = 1000000000; 
	size_t best_index = 0;
	for (size_t i = 0; i < C.n; i++) {
		counts->strings++;
		int d = editDistance((char*)query, (char*)C.first_element[i].key,
			(int)strlen(query), (int)strlen(C.first_element[i].key));
		if (d < best_distance || (d == best_distance && strcmp(C.first_element[i].key, C.first_element[best_index].key) < 0)) {
			best_distance = d;
			best_index = i;
		}
	}

	out->count = C.first_element[best_index].count;
	out->items = malloc(out->count * sizeof *out->items);
	assert(out->items);
	for (size_t i = 0; i < out->count; i++) {
		out->items[i] = C.first_element[best_index].items[i];
	}

	free(C.first_element);
	return 0;
}
