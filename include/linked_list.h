#ifndef LINKED_LIST_H
#define LINKED_LIST_H

// singly linked list for AddressRecord*
// preserves file order by appending new nodes at the tail in O(1)

#include <stddef.h>
#include "records.h" 

//one node in the singly-linked list
typedef struct ListNode {
    AddressRecord *rec; //pointer to an AddressRecord
    struct ListNode *next; //next node
} ListNode;

typedef struct LinkedList {
    ListNode *head;
    ListNode *tail;
} LinkedList;

void list_init(LinkedList *list); //initialise empty list

int list_append(LinkedList *list, AddressRecord *rec); //append AddressRecord* at the end and preserve original input order

void list_free(LinkedList *list, int free_records); //free the whole list. 
//If free_records != 0, also free each AddressRecord using record_free(). LinkedList struct itself is not freed (stack-owned)

#endif
