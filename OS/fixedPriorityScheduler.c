#include "fixedPriorityScheduler.h"
#include "stm32f4xx.h"

/* Scheduling Functions */
static void init(void);
static OS_TCB_t const * scheduler(void);
static void addTask(OS_TCB_t * const tcb);
static void taskExit(OS_TCB_t * const tcb);
static void waitCallback(OS_TCB_t * const task, uint32_t const checkCode);
static void notifyCallback(OS_TCB_t * const task);
static void sleepCallback(OS_TCB_t *task, uint32_t const wakeupTime);

/* Utility functions for this scheduler */
static void removeFromRunnable(OS_TCB_t * const task);
static void wakeupTasks(void); 


/* Task Queues 
 * The Tail and Head are arrays which point to the tail/head of a linked list for each priority level.
 * FIXED_PRIORITY_LOWEST is always the last priority in the enumeration and as such will have the highest value
 * The value will be = number of items in the enumaration - 1
*/
//static OS_TCB_t *runnable_tail[FIXED_PRIORITY_LOWEST+1] = {NULL};
//static OS_TCB_t *runnable_head[FIXED_PRIORITY_LOWEST+1] = {NULL};
//static OS_TCB_t *sleeping_tasks_head = NULL; // A singly linked list, sorted by wakeup time. TODO: Consider using beap
static OS_TaskList_t runnable_tasks[FIXED_PRIORITY_LOWEST+1];
static OS_TaskList_t sleeping_tasks;

OS_Scheduler_t const fixedPriorityScheduler = {
	.preemptive = 1,
	.scheduler_init = init,
	.scheduler_callback = scheduler,
	.addtask_callback = addTask,
	.taskexit_callback = taskExit,
	.wait_callback = waitCallback,
	.notify_callback = notifyCallback,
	.sleep_callback = sleepCallback
};

static void init() {
	for (int i=0; i <= FIXED_PRIORITY_LOWEST; i++){
		OS_initialiseList(&(runnable_tasks[i]));
	}
	OS_initialiseList(&sleeping_tasks);
}

/* 
 * Returns the TCB of the highest priority runnable task or OS_idleTCB_p if 
 * there is no more tasks which can be run.
 */
static OS_TCB_t const * scheduler() {
	wakeupTasks();
	for (int i=0; i <= FIXED_PRIORITY_LOWEST; i++) {
		if (runnable_tasks[i].head != NULL){
			return runnable_tasks[i].head;
		}
	}

	return OS_idleTCB_p;
}

/* Adds a task to the list of runnable tasks 
 * For adding entirely new tasks to the queue or for putting woken up tasks back into the runnable queues
*/
static void addTask(OS_TCB_t * const newTask) {
	uint32_t taskPriority = newTask->priority;
	OS_appendToList(&(runnable_tasks[taskPriority]), newTask);
//	if (runnable_tail[taskPriority] != NULL) {
//		runnable_tail[taskPriority]->next = newTask;
//		newTask->prev = runnable_tail[taskPriority];
//		runnable_tail[taskPriority] = newTask;
//	} else {
//		runnable_head[taskPriority] = newTask;
//		runnable_tail[taskPriority] = newTask;
//		newTask->prev = NULL;
//		newTask->next = NULL;
//	}
}

/* A callback used to indicate that a particular task has finished */
static void taskExit(OS_TCB_t *task) {
	removeFromRunnable(task);
}

/* Puts the currently running task into a waiting state, where it will not run
 * until it is notified.
*/
static void waitCallback(OS_TCB_t * const task, uint32_t const checkCode) {
	if (checkCode == OS_checkCode()) {
		removeFromRunnable(task);
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk; // Invoke Scheduler
	}
}

/* Sends a wakeup signal with a particular reason to sleeping tasks, 
 * causing matching tasks to wakeup.
*/
static void notifyCallback(OS_TCB_t * const task) {
	addTask(task);
}

/* Removes the given task from the runnable tasks lists and inserts it into the sleeping tasks list in the appropriate position
with regard to its wakeup time.  Sorting into this list is O(n) :( */
static void sleepCallback(OS_TCB_t * const task, uint32_t const wakeupTime) {
	removeFromRunnable(task);
	task->wakeupTime = wakeupTime;

	OS_TCB_t *prevInList = sleeping_tasks.head;
	OS_TCB_t *listPointer = sleeping_tasks.head;
	if (listPointer == NULL || IS_AFTER(wakeupTime, listPointer->wakeupTime) == 0) {
		task->next = listPointer;
		sleeping_tasks.head = task;
	} else {
		while (!IS_AFTER(wakeupTime, listPointer->wakeupTime) && listPointer->next != NULL) {
			prevInList = listPointer;
			listPointer = listPointer->next;
		}
		task->next = listPointer->next;
		prevInList->next = task;
	} 
}


/* Will wakeup any sleeping tasks who need to be and adds them to the end of the appropriate runnable queue */
static void wakeupTasks() {
	// Because the sleeping list is sorted by soonest wakup first, once we reach the first task which isnt ready yet, then we can stop.
	uint32_t t = OS_elapsedTicks();
	if (sleeping_tasks.head != NULL) {
		while (IS_AFTER(OS_elapsedTicks(), sleeping_tasks.head->wakeupTime)) {
			
			OS_TCB_t *readyTask = sleeping_tasks.head;
			OS_removeFromList(&sleeping_tasks, readyTask);
			addTask(readyTask);
		}
	}
}

static void removeFromRunnable(OS_TCB_t * const task) {
	OS_removeFromList(&(runnable_tasks[task->priority]), task);
//	if (runnable_head[task->priority] == task) {
//		runnable_head[task->priority] = task->next;
//	}
//	
//	if (task->next != NULL) {
//		task->next->prev = task->prev;
//	}
//	
//	if (task->prev != NULL) {
//		task->prev->next = task->next;
//	}
//	
//	if (runnable_tail[task->priority] == task) {
//		runnable_tail[task->priority] = task->prev;
//	}
//	task->next = NULL;
//	task->prev = NULL;
}
