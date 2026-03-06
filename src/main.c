#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h" 
#include "records.h"
#include "csv_loader.h"
#include "linked_list.h"
#include "radix_tree.h"
#include "bit.h"

int run_stage1(const char *csv_path, const char *out_path);
int run_stage2(const char *csv_path, const char *out_path);

// Removes newline character from a line.
void remove_newlines(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n && s[n - 1] == '\n') {
        s[n - 1] = '\0';
        n--;
    }
}

// Main
int main (int argc, char **argv) {
    
    if (argc != 4) {
        return 1;
    }

    int stage = atoi(argv[1]);
    const char *csv_path = argv[2];
    const char *out_path = argv[3];

    if (stage == 1) {
        return run_stage1(csv_path, out_path);
    }
    else if (stage == 2) {
        return run_stage2(csv_path, out_path);
    }
    else {
        return 1;
    }

}

// Run Stage 1
int run_stage1(const char *csv_path, const char *out_path) {
    // Open output file
    FILE *fout = fopen(out_path, "w"); 
    if (!fout) {
        return 1;
    }

    // Build dictionary (linked list and headers) from CSV
    Dictionary dict;
    dict_init(&dict);
    if (dict_load(&dict, csv_path) != 0) {
        fclose(fout);
        dict_free(&dict);
        return 1;
    }

    // Read queries from stdin
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        remove_newlines(line);

        // Write query line to output file as a header line
        fputs(line, fout);
        fputc('\n', fout);

        // Search dictionary and gather results as well as 
        // count comparisons
        SearchResults results;
        CompareCounts counts;
        if (dict_search(&dict, line, &results, &counts) != 0) {
            fclose(fout);
            dict_free(&dict);
            return 1;
        }

        // Print all matches to the output file
        if (results.count == 0) {
            fputs("NOTFOUND\n", fout);
        }
        else {
            for (size_t i = 0; i < results.count; i++) {
            record_print(results.items[i], fout, dict.headers);
            }
        }

        // Print the counts line to stdout in the required format
        printf("%s --> %zu records found - comparisons: b%lu n%lu s%lu\n", 
            line, results.count, counts.bits, counts.nodes, counts.strings);

        search_results_free(&results);
    }

    fclose(fout);
        dict_free(&dict);
        return 0;
}


// Run Stage 2
int run_stage2(const char *csv_path, const char *out_path) {
    // Open output file
    FILE *fout = fopen(out_path, "w"); 
    if (!fout) {
        return 1;
    }

    LinkedList list;
    list_init(&list);
    char *headers[ADDRESS_FIELD_COUNT] = {0};
    
    if (csv_load_file(csv_path, &list, headers) != 0) {
        fclose(fout);
        for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
            free(headers[i]);
        }
        list_free(&list, 0);
        return 1;
    }

    // Build radix tree from EZI_ADD keys
    RadixTree tree;
    rt_init(&tree);
    for (ListNode *n = list.head; n; n = n->next) {
        if (rt_insert(&tree, n->rec) != 0) {
            rt_free(&tree);
            list_free(&list, 1);
            for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
                free(headers[i]);
            }
            fclose(fout);
            return 1;
        }
    }

    // Process queries from stdin
    char line[1024];
    while(fgets(line, sizeof line, stdin)) {
        remove_newlines(line);
        fputs(line, fout);
        fputc('\n', fout);

        SearchResults results;
        CompareCounts counts = (CompareCounts) {0};
        if (rt_search_best(&tree, line, &results, &counts) != 0) {
            rt_free(&tree);
            list_free(&list, 1);
            for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
                free(headers[i]);
            }
            fclose(fout);
            return 1;
        }

        if (results.count == 0) {
            fputs("NOTFOUND\n", fout);
        }
        else {
            for (size_t i = 0; i < results.count; i++) {
                record_print(results.items[i], fout, headers);
            }
        }

        printf("%s --> %zu records found - comparisons: b%lu n%lu s%lu\n",
            line, results.count, counts.bits, counts.nodes, counts.strings);

        search_results_free(&results);
    }

    rt_free(&tree);
    list_free(&list, 1);
    for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
        free(headers[i]);
    }
    
    fclose(fout);
    return 0;
}



