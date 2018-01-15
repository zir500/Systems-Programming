#include "mutex.h"
#include "os.h"
#include "stm32f4xx.h"          


/* Initialises a mutex */
void OS_mutex_init(OS_mutex_t * const mutex){
	mutex->counter = 0;
	mutex->owner = 0;
	OS_initialiseList(&mutex->waitingTasks);
}

/* Attempts to aquire the given mutex.  
 * If the mutex isnt owned, then the ownership passes to thie calling task. 
 * If the mutex is already owned by this task then it will lay an additional claim to the mutex.
 * If the mutex is owned by some other task, then the current task will yeild and wait untill the mutex is released.
*/
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
			if (!OS_wait(checkCode)){ 
				OS_removeFromList(&mutex->waitingTasks, OS_currentTCB());
			}
		}
	}
}

/* Decreases the number of claims to this mutex.  
 * If there are no more claims to this mutex then it passes ownership on to the next task which is waiting.
 * Or just clears to 0 if there are no task waiting.
*/
void OS_mutex_release(OS_mutex_t * const mutex){
	if (mutex->owner == OS_currentTCB()){
		mutex->counter--;
		if (mutex->counter == 0){
			if (mutex->waitingTasks.head != NULL){
				OS_TCB_t *nextTask =  mutex->waitingTasks.head;
				OS_removeFromList(&mutex->waitingTasks, mutex->waitingTasks.head);
				mutex->owner = nextTask;
				OS_notify(nextTask); 
			} else {
				mutex->owner = 0;
			}
		}
	}
}
