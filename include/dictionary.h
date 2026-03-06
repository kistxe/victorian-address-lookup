#ifndef DICTIONARY_H
#define DICTIONARY_H

//has linked list of records and a copy of csv header names
// and searches by EZI_ADD

#include <stddef.h>
#include "records.h" 
#include "linked_list.h" 

typedef struct CompareCounts {
    unsigned long bits; //for bit comparisons
    unsigned long nodes; //number of nodes accessed (list size)
    unsigned long strings; //number of string compares
} CompareCounts;

typedef struct SearchResults {
    AddressRecord **items; 
    size_t count; //number of matches
    size_t capacity; //internal capacity for items[]
} SearchResults;

typedef struct Dictionary {
    LinkedList list;
    char *headers[ADDRESS_FIELD_COUNT];
} Dictionary;

void dict_init(Dictionary *dict);
int dict_load(Dictionary *dict, const char *csv_path); //0 is ok, -1 fail
void dict_free(Dictionary *dict);

int dict_search(const Dictionary *dict, const char *key, SearchResults *out_results, CompareCounts *out_counts);

void search_results_init(SearchResults *r);
void search_results_free(SearchResults *res);

#endif
