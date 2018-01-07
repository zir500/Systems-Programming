#ifndef MUTEX_H
#define MUTEX_H

#include "task.h"

typedef struct {
	/* The TCB of the task which currently holds the mutex */
	OS_TCB_t *owner;
	
	/* The number of times the task which holds this mutex has aquired it */
	uint32_t counter;
	
	/* List of tasks which a waiting for this mutex */
	OS_TaskList_t waitingTasks;
} OS_mutex_t;

/* Initialises the given mutex includeing setting the owner and counter to 0 */
void OS_mutex_init(OS_mutex_t * const mutex);

/* Aquires the given mutex.  Blocking, will wait for the mutex to be availiable
 * before returning. Will wait indefinately if the mutex cannot be aquired.
*/
void OS_mutex_aquire(OS_mutex_t * const mutex);

/* Releases a claim to the given mutex.  
 * Once the mutex has been released as many times as it was quired by a task, 
 * the owner of the mutex will be reset to 0 
*/
void OS_mutex_release(OS_mutex_t * const mutex);





#endif // MUTEX_H
