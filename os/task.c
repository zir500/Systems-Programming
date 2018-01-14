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
	if (listPointer == NULL || listPointer->priority >= task->priority) {
		task->next = listPointer;
		list->head = task;
	} else {
		while ((listPointer->priority >= task->priority) && (listPointer->next != NULL)) {
			prevInList = listPointer;
			listPointer = listPointer->next;
		}
		task->next = listPointer->next;
		prevInList->next = task;
	} 
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
	task->next = NULL;
	task->prev = NULL;
}
