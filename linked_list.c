#include "linkedlist.h"

void linkedListInit(linkedList_t * const linkedList) {
	linkedList->head = NULL;
	linkedList->tail = NULL;
}

void linkedListAdd(linkedList_t * const linkedList, linkedListElement_t const * const element) {
	if (linkedList->tail != NULL) {
		// Store a reference to the head
		// Set the next of the last element so that it points to the element being added
		// Set the new element's prev to the current last element
		// Set the new element's next to the head
		// Move the tail to the new element
		// Move te head's prev pointer to point at the new final element
		
		
		linkedListElement_t * head = linkedList->tail->next;	
		*(linkedList)->tail->next = element;
		element->prev = linkedList->tail;
		element->next = head;
		*(linkedList)->tail = element;
		head->prev = element;
		
	} else {
		// This element is being added to an empty queue
		*(linkedList)->tail = element;
		element->prev = element;
		element->next = element;
	}
}

void linkedListRemove(linkedList_t * linkedList, linkedListElement_t const * const element) {
	element->next->prev = element->prev;
	element->prev->next = element->next;
	
	if (*(linkedList)->tail == element) {
		*(linkedList)->tail = element->prev;
	}
}

linkedListElement_t linkedListNext(linkedList_t * const linkedList) {
	
}
