#ifndef CSV_LOADER_H
#define CSV_LOADER_H

//reads the file once, parses the header line, returns a duplicated copy of col name
//for each row, creates an AddressRecord and appends to given list

#include "linked_list.h"
#include "records.h"

int csv_load_file(const char *filename, LinkedList *list_out, char *headers_out[ADDRESS_FIELD_COUNT]);

#endif
