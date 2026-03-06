#include "dictionary.h" 
#include "csv_loader.h" 
#include "bit.h" 
#include <string.h>
#include <stdlib.h>

// Creates a growing array for SearchResults.
int results_array(SearchResults *r, AddressRecord *rec) {
    if (r->count == r->capacity) {
        size_t new_capacity = 8U;
        if (r->capacity != 0) {
            new_capacity = r->capacity * 2;
        } 

        AddressRecord **p = (AddressRecord **)realloc(r->items, 
            new_capacity * sizeof *r->items);

        if (!p) return -1;
        r->items = p;
        r->capacity = new_capacity;
    }
    r->items[r->count++] = rec;
    return 0;
}

/* Bitwise equality compare that counts the number of bit 
comparisons and stops counting at the first mismatch. If equal, 
counts all bits including the null terminator */
int bitwise_count(const char *a, const char *b, unsigned long *bit_count) {
    size_t length_a = strlen(a);
    size_t length_b = strlen(b);

    // Maximum number of bits that may need to be checked, including
    // the null byte.
    size_t max_len;
    if (length_a > length_b) {
        max_len = length_a;
    }
    else {
        max_len = length_b;
    }
    size_t max_bits = (max_len + 1U) * 8U;

    for (size_t i = 0; i < max_bits; i++) {
        int bit_a = getBit((char *)a, (unsigned int)i);
        int bit_b = getBit((char *)b, (unsigned int)i);
        (*bit_count)++;
        if (bit_a != bit_b) {
            // Mismatch found
            return 0; 
        }
    }
    return 1;
}

void search_results_init(SearchResults *r) {
    r->items = NULL;
    r->count = 0;
    r->capacity = 0;
}

void search_results_free(SearchResults *r) {
    free(r->items);
    r->items = NULL;
    r->count = r->capacity = 0;
}

void dict_init(Dictionary *d) {
    list_init(&d->list);
    for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
        d->headers[i] = NULL;
    }
}

// Passes the list and header array to the CSV loader.
int dict_load(Dictionary *d, const char *csv_path) {
    if (csv_load_file(csv_path, &d->list, d->headers) != 0) {
        return -1;
    }
    return 0;
}

void dict_free(Dictionary *d) {
    // Free all records and nodes
    list_free(&d->list, 1);

    // Free header strings
    for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
        free(d->headers[i]);
        d->headers[i] = NULL;
    }
}

// Linear scan of the list and counts node accesses, string 
// comparisons and bit comparisons.
int dict_search(const Dictionary *d, const char *key, 
        SearchResults *out_results, CompareCounts *out_counts) {
    
    search_results_init(out_results);
    out_counts->bits = 0;
    out_counts->nodes = 0;
    out_counts->strings = 0;
    const char *query = key;

    for (ListNode *n = d->list.head; n; n = n->next) {
        // add one node visit
        out_counts->nodes++;

        // add string comparison
        out_counts->strings++;
        
        const char *rec_key = record_get_key(n->rec);

        unsigned long bits_added = 0;
        int equal = bitwise_count(rec_key, query, &bits_added);
        out_counts->bits += bits_added;

        if(equal) {
            if (results_array(out_results, n->rec) != 0) {
                search_results_free(out_results);
                return -1;
            }
        }
    }
    return 0;
}



