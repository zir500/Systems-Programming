#include "mutex.h"
#include "os.h"
#include "stm32f4xx.h"          



void OS_mutex_init(OS_mutex_t * const mutex){
	mutex->counter = 0;
	mutex->owner = 0;
	OS_initialiseList(&mutex->waitingTasks);
}

void OS_mutex_aquire(OS_mutex_t * const mutex){
	while (1){
		uint32_t const mutex_owner = __LDREXW ((uint32_t*)mutex);
		uint32_t checkCode = OS_checkCode();
		
		if (mutex_owner == 0){
			if (__STREXW ((uint32_t)OS_currentTCB(), (uint32_t*)mutex)) {
				// We have claimed the mutex
				mutex->counter++;
				return;
			}
		} else if (mutex_owner == (uint32_t)OS_currentTCB()){
			// We already own the mutex
			mutex->counter++;
			return;
		} else {
			// Someone else holds the mutex, wait for it.
			OS_addToListByPriority(&mutex->waitingTasks, OS_currentTCB()); 
			OS_wait(checkCode);
		}
	}
}

void OS_mutex_release(OS_mutex_t * const mutex){
	if (mutex->owner == OS_currentTCB()){
		mutex->counter--;
		if (mutex->counter == 0){
			mutex->owner = 0;
			// Notify the next in the queue
			if (mutex->waitingTasks.head != NULL){
				OS_TCB_t *nextTask =  mutex->waitingTasks.head;
				OS_removeFromList(&mutex->waitingTasks, mutex->waitingTasks.head);
				/* What if a context switch happens here?, the true heir to this mutex has been removed, 
				the mutex has no owner, someone else could sneak in and claim it.  
				Eventually the task which was releasing this mutex will be run again, 
				Then the true heir will wake up, realise the mutex isnt avaliable and go back to sleep... I'm ok with this.*/
				OS_notify(nextTask); 
			}
		}
	}
}
