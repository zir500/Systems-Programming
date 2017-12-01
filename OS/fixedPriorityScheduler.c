#include "fixedPriorityScheduler.h"

/* Scheduling Functions */
static void init(void);
static OS_TCB_t const * scheduler(void);
static void addTask(OS_TCB_t * const tcb);
static void taskExit(OS_TCB_t * const tcb);
static void waitCallback(void * const reason, uint32_t const checkCode);
static void notifyCallback(void * const reason);
static void sleepCallback(OS_TCB_t *task, uint32_t const wakeupTime);

/* Utility functions for this scheduler */
static void removeFromRunnable(OS_TCB_t *task);
static void wakeupTasks(void); 


/* Task Queues */
static OS_TCB_t *runnable_tail[FIXED_PRIORITY_LOWEST+1] = {NULL};
static OS_TCB_t *runnable_head[FIXED_PRIORITY_LOWEST+1] = {NULL};
static OS_TCB_t *waiting_tasks_head = NULL; // A singly linked list, sorted by wakeup time. TODO: Consider using beap


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
	
}

/* 
 * Returns the TCB of the highest priority runnable task
 */
static OS_TCB_t const * scheduler() {
	wakeupTasks();
	for (int i=0; i <= FIXED_PRIORITY_LOWEST; i++) {
		if (runnable_head[i] != NULL){
			return runnable_head[i];
		}
	}

	return OS_idleTCB_p;
}

static void addTask(OS_TCB_t * const newTask) {
	uint32_t taskPriority = newTask->priority;
	if (runnable_tail[taskPriority] != NULL) {
		runnable_tail[taskPriority]->next = newTask;
		newTask->prev = runnable_tail[taskPriority];
		runnable_tail[taskPriority] = newTask;
	} else {
		runnable_head[taskPriority] = newTask;
		runnable_tail[taskPriority] = newTask;
		newTask->prev = runnable_tail[taskPriority];
	}
}

static void taskExit(OS_TCB_t *task) {
	removeFromRunnable(task);
}

static void waitCallback(void * const reason, uint32_t const checkCode) {
	
}

static void notifyCallback(void * const reason) {
	
}

/* Removes the given task from the runnable tasks lists and inserts it into the sleeping tasks list in the appropriate position
with rehard to its wakeup time.  Sorting into this list is O(n) :( */
static void sleepCallback(OS_TCB_t *task, uint32_t const wakeupTime) {
	removeFromRunnable(task);
	
	OS_TCB_t *prevInList = waiting_tasks_head;
	OS_TCB_t *listPointer = waiting_tasks_head;
	while (IS_AFTER(wakeupTime, listPointer->data) && listPointer != NULL) {
		prevInList = listPointer;
		listPointer = listPointer->next;
	}
	
	if (waiting_tasks_head != NULL) {
		prevInList->next = task;
	} else {
		waiting_tasks_head = task;
	}
	
	task->next = listPointer;
	task->data = wakeupTime;
}


/* Will wakeup any sleeping tasks who need to be and adds them to the end of the appropriate runnable queue */
static void wakeupTasks() {
	// Because the sleeping list is sorted by soonest wakup first, once we reach the first task which isnt ready yet, then we can stop.
	uint32_t t = OS_elapsedTicks();
	if (waiting_tasks_head != NULL) {
		while (IS_AFTER(OS_elapsedTicks(), waiting_tasks_head->data)) {
			waiting_tasks_head->state &= ~TASK_STATE_SLEEP;
			addTask(waiting_tasks_head);
			
			waiting_tasks_head = waiting_tasks_head->next; // Move head to the next in the list.
		}
	}
}

static void removeFromRunnable(OS_TCB_t *task) {
	if (task->next != NULL && task->prev != NULL){
		task->next->prev = task->prev;
		task->prev->next = task->next;
		
		if (runnable_head[task->priority] == task) {
			runnable_head[task->priority] = task->next;
		} 
		if (runnable_tail[task->priority] == task){
			runnable_tail[task->priority] = task->prev;
		}
	} else {
		runnable_head[task->priority] = NULL;
	}
}
