#include "records.h"
#include <stdlib.h>
#include <string.h>

// Duplicates strings.
char *str_dup(const char *s) {
    if (!s) s = "";
    size_t n = strlen(s);
    char *p = (char *)malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n + 1);
    return p;
}

// Frees the first fields entries.
void free_fields(AddressRecord *rec, int fields) {
    if (!rec) return;
    for (int i = 0; i < fields; i++) {
        free(rec->fields[i]);
        rec->fields[i] = NULL;
    }
}

// Creates AddressRecord by duplicating field strings.
AddressRecord *record_create_from_fields(const char 
    *cols[ADDRESS_FIELD_COUNT]) {

    AddressRecord *rec = (AddressRecord *)calloc(1, sizeof(AddressRecord));
    if (!rec) return NULL;
    for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
        rec->fields[i] = str_dup(cols[i]);
        if (!rec->fields[i]) {
            free_fields(rec, i);
            free(rec);
            return NULL;
        }
    }
    return rec;
}

// Frees all allocated strings and the record itself.
void record_free(AddressRecord *rec) {
    if (!rec) return;
    for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
        free(rec->fields[i]);
        rec->fields[i] = NULL;
    }
    free(rec);
}

// Returns the EZI_ADD string as the search key.
const char *record_get_key(const AddressRecord *rec) {
    if (rec) {
        return rec->fields[EZI_ADD_INDEX];
    }
    return "";
}

// Print one record in the required format.
void record_print(const AddressRecord *rec, FILE *out, 
    char *const headers[ADDRESS_FIELD_COUNT]) {
    long double t; 

    if (!rec || !out || !headers) return;
    fputs("--> ", out);
    for (int i = 0; i < ADDRESS_FIELD_COUNT; i++) {
        // x and y stored as strings but printed as decimals 
        // using sscanf
        if (i == X_INDEX || i == Y_INDEX) {
            const char *s = "";
            if (rec->fields[i] != NULL) {
                s = rec->fields[i];
            }

            if (s[0] != '\0') {
                if (sscanf(s, "%Lf", &t) == 1) {
                    fprintf(out, "%s: %.5Lf || ", headers[i], t);
                    continue;
                }
            }
        }
        else {
            fprintf(out, "%s: %s || ", headers[i], rec->fields[i]);
        }
    }
    fputc('\n', out);
}

