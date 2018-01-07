#include "task.h"

void OS_initialiseList(OS_TaskList_t * const list){
	list->head = NULL;
	list->tail = NULL;
}


void OS_appendToList(OS_TaskList_t * const list, OS_TCB_t * const task) {
	if (list->tail != NULL) {
		list->tail->next = task;
		task->prev = list->tail;
		list->tail = task;
	} else {
		list->head = task;
		list->tail = task;
		task->prev = NULL;
		task->next = NULL;
	}
}

void OS_addToListByPriority(OS_TaskList_t * const list, OS_TCB_t * const task) {
	OS_TCB_t *prevInList = list->head;
	OS_TCB_t *listPointer = list->head;
	while (listPointer != NULL) {
		if (listPointer->priority <= task->priority) {
			prevInList = listPointer;
			listPointer = listPointer->next;
		}
	}
	
	if (list->head != NULL) {
		prevInList->next = task;
	} else {
		list->head = task;
	}
	
	task->next = listPointer;
}


void OS_removeFromList(OS_TaskList_t * const list, OS_TCB_t * const task) {
	if (list->head == task) {
		list->head = task->next;
	}
	
	if (task->next != NULL) {
		task->next->prev = task->prev;
	}
	
	if (task->prev != NULL) {
		task->prev->next = task->next;
	}
	
	if (list->tail == task) {
		list->tail = task->prev;
	}
}