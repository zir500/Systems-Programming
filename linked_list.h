#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/* Circular linked list  */

/* An element in a linked list. */
typedef struct linkedListElement {
	struct linkedListElement *next;
	struct linkedListElement *prev; // To make random removal easier
	void *data;
} linkedListElement_t;

/* Defines one of these lists.  
	(Although this could be done with just a typedef to a pointer of linkedListElement_t doing this as a structure seems more transparent) 
*/
typedef struct {
	linkedListElement_t* tail;
} linkedList_t; 

/* Initialises a Linked list structure NB Codiser removs*/
void linkedListInit(linkedList_t * const linkedList);

/* Adds an element to the end of the linked list */
void linkedListAdd(linkedList_t * const linkedList, linkedListElement_t const * const element);

/* Removes the given element from the list (this can be any element) */
void linkedListRemove(linkedList_t * const linkedList, linkedListElement_t const * const element);

linkedListElement_t linkedListNext(linkedList_t * const linkedList);


#endif // LINKED_LIST_H
