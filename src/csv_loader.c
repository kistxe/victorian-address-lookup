#include "csv_loader.h" 
#include "records.h" 
#include "linked_list.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Duplicates strings.
char *str_dup1(const char *s) {
    if (!s) s = "";
    size_t n = strlen(s);
    char *p = (char *)malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n + 1);
    return p;
}

// Removes newline character from a line.
void remove_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n && s[n - 1] == '\n') {
        s[n - 1] = '\0';
        n--;
    }
}

// Splits the CSV line on commas.
int split_csv_line(char *line, char *out[ADDRESS_FIELD_COUNT]) {
    int count = 0;
    char *p = line;
    while (count < ADDRESS_FIELD_COUNT) {
        char *comma = strchr(p, ',');
        if (comma) {
            *comma = '\0';
            out[count++] = p;
            p = comma + 1;
        }
        else {
            // When no comma is left, take the rest as the last field.
            out[count++] = p;
            break;
        }
    }
    return count;
}

// Reads the CSV file and appends into the linked list.
int csv_load_file(const char *path, LinkedList *list, 
    char *headers[ADDRESS_FIELD_COUNT]) {
    
    FILE *fp = fopen(path, "r");
    if (!fp) return -1; 

    // Maximum readable characters.
    char line[1024]; 

    if (!fgets(line, sizeof line, fp)) {
        fclose(fp);
        return -1;
    }

    remove_newline(line);

    // Parse headers.
    char *header_pointers[ADDRESS_FIELD_COUNT] = {0}; 
    int n = split_csv_line(line, header_pointers);
    if (n != ADDRESS_FIELD_COUNT) {
        fclose(fp);
        return -1;
    }

    for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
        char *p = header_pointers[i];
        if (p) {
            headers[i] = str_dup1(p);
        }
        else {
            headers[i] = str_dup1("");
        }
        if (!headers[i]) {
            for (int j = 0; j < i; j++) {
                free(headers[j]);
                headers[j] = NULL;
            }
            fclose(fp);
            return -1;
        }
    }

    // Read and parse each data row.
    while (fgets(line, sizeof line, fp)) {
        remove_newline(line);

        char *field_pointers[ADDRESS_FIELD_COUNT] = {0};
        int n = split_csv_line(line, field_pointers);
        // Skip blank lines
        if (n <= 0) continue; 

        // Builds a column array of non-null pointers.
        const char *cols[ADDRESS_FIELD_COUNT];
        for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
            char *p;
            if (i < n) {
                p = field_pointers[i];
            }
            else {
                p = NULL;
            }
            
            if (p) {
                cols[i] = p;
            }
            else {
                cols[i] = "";
            }
        }

        AddressRecord *rec = record_create_from_fields(cols);
        if (!rec) {
            fclose(fp);
            return -1;
        }

        if (list_append(list, rec) != 0) {
            record_free(rec);
            fclose(fp);
            return -1;
        }
    }
    fclose(fp);
    return 0;
}

