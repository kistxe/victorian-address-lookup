#include "linked_list.h"
#include <stdlib.h>

// Initialise an empty list
void list_init(LinkedList *list) {
    list->head = list->tail = NULL;
}

// Appends to tail of the list.
int list_append(LinkedList *list, AddressRecord *rec) {
    ListNode *n = (ListNode *)malloc(sizeof *n);

    // Malloc fails.
    if (!n) return -1;

    n->rec = rec;
    n->next = NULL;

    if (!list->head) {
        list->head = list->tail = n;
    }
    else {
        list->tail->next = n;
        list->tail = n;
    }

    // Successfully appends.
    return 0;
}

// Frees all nodes.
void list_free(LinkedList *list, int free_records) {
    ListNode *curr = list->head;
    while (curr) {
        ListNode *next = curr->next;
        if (free_records && curr->rec) {
            record_free(curr->rec);
        }
        free(curr);
        curr = next;
    }
    list->head = list->tail = NULL;
}

