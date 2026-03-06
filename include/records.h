#ifndef RECORDS_H
#define RECORDS_H

#include <stdio.h>
#include <stddef.h>

#define ADDRESS_FIELD_COUNT 35
#define EZI_ADD_INDEX 1
#define X_INDEX 33
#define Y_INDEX 34

// address records stored as heap-allocated strings
typedef struct AddressRecord {
	char *fields[ADDRESS_FIELD_COUNT];
} AddressRecord;

// copy each of the 35 strings into new record
AddressRecord *record_create_from_fields(const char *cols[ADDRESS_FIELD_COUNT]);

void record_free(AddressRecord *rec);

//borrowed pointer to EZI_ADD key inside the record
const char *record_get_key(const AddressRecord *rec);

// print record line for stage 1
void record_print(const AddressRecord *rec, FILE *out, char *const headers[ADDRESS_FIELD_COUNT]);

#endif
