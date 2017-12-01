#include "simpleRoundRobin.h"
#include "stm32f4xx.h"

/* This is an implementation of an extremely simple (and inefficient!) round-robin scheduler.

   An array of pointers to TCBs is declared, and when tasks are added they are inserted into
	 this array.  When tasks finish, they are removed from the array (pointers are set back to
	 zero).  When the scheduler is invoked, it simply looks for the next available non-zero
	 pointer in the array, and returns it.  If there are no non-zero TCB pointers in the array,
	 a pointer to the idle task is returned instead.
	 
	 The inefficiency in this implementation arises mainly from the way arrays are used for list
	 storage.  If the array is large and the number of tasks is small, the scheduler will spend
	 a lot of time looking through the array each time it is called. */

/* Prototypes (functions are static, so these aren't in the header file) */
static OS_TCB_t const * simpleRoundRobin_scheduler(void);
static void simpleRoundRobin_addTask(OS_TCB_t * const tcb);
static void simpleRoundRobin_taskExit(OS_TCB_t * const tcb);
static void simpleRoundRobin_wait(void * const reason, uint32_t const checkCode);
static void simpleRoundRobin_notify(void * const reason);

static OS_TCB_t * tasks[SIMPLE_RR_MAX_TASKS] = {0};

/* Scheduler block for the simple round-robin */
OS_Scheduler_t const simpleRoundRobinScheduler = {
	.preemptive = 1,
	.scheduler_callback = simpleRoundRobin_scheduler,
	.addtask_callback = simpleRoundRobin_addTask,
	.taskexit_callback = simpleRoundRobin_taskExit,
	.wait_callback = simpleRoundRobin_wait,
	.notify_callback = simpleRoundRobin_notify
};


/* Round-robin scheduler callback */
static OS_TCB_t const * simpleRoundRobin_scheduler(void) {
	static int i = 0;
	// Clear the yield flag if it's set - we simply don't care
	OS_currentTCB()->state &= ~TASK_STATE_YIELD;
	for (int j = 1; j <= SIMPLE_RR_MAX_TASKS; j++) {
		i = (i + 1) % SIMPLE_RR_MAX_TASKS;
		if (tasks[i] != 0) {
			
			if (tasks[i]->state & TASK_STATE_SLEEP) {
				// By considering the timer to be signed, any values near the maximum value it could hold
				// will be considered -ve.  If the wakeup time is in the +ve rance but the current time has jumped over it and is now
				// in the -ve range (ie > 2^31-1) then we end up doing -ve - +ve with 2 'large' value which will overflow the operation.
				// The result is the carry bit being set and the value being set guarenteed to be +Ve
				// This does however, cut the longest allowable sleep time from 2^32-1 ticks to 2^31-1 ticks (but thats still ~25 days)
				if ((int32_t)OS_elapsedTicks() - (int32_t)tasks[i]->data > 0) {
					tasks[i]->state &= ~TASK_STATE_SLEEP;
					return tasks[i];
				}
			} else if (!(tasks[i]->state & TASK_STATE_WAITING)) {
				return tasks[i];
			}
		}
	}
	// No tasks in the list, so return the idle task
	return OS_idleTCB_p;
}

/* 'Add task' callback */
static void simpleRoundRobin_addTask(OS_TCB_t * const tcb) {
	for (int i = 0; i < SIMPLE_RR_MAX_TASKS; i++) {
		if (tasks[i] == 0) {
			tasks[i] = tcb;
			return;
		}
	}
	// If we get here, there are no free TCB slots, so the task just won't be added
}

/* 'Task exit' callback */
static void simpleRoundRobin_taskExit(OS_TCB_t * const tcb) {
	// Remove the given TCB from the list of tasks so it won't be run again
	for (int i = 0; i < SIMPLE_RR_MAX_TASKS; i++) {
		if (tasks[i] == tcb) {
			tasks[i] = 0;
		}
	}	
}

/* Causes current task to wait until a notify callback is issued with the same reason code.
	 Halts the current task and sets PendSV bit */
static void simpleRoundRobin_wait(void * const reason, uint32_t const checkCode){
	OS_TCB_t *currentTask = OS_currentTCB();
	while (1){
		uint32_t current_task_state = __LDREXW(&(currentTask->state));
		
		if (checkCode == OS_checkCode()) {
			current_task_state |= TASK_STATE_WAITING;
			currentTask->data = (uint32_t) reason;
			if (__STREXW(current_task_state, &(currentTask->state))){
				SCB->ICSR = SCB_ICSR_ISRPENDING_Msk;
				return;
			} 
		}
	}
}

/* Allows any tasks which were waiting for the reason code passed to this callback to run again 
	 next time the scheduler is invoked */
static void simpleRoundRobin_notify(void * const reason){
	for (int i = 0; i < SIMPLE_RR_MAX_TASKS; i++) {
		if ((tasks[i]->state & TASK_STATE_WAITING) && (tasks[i]->data == (uint32_t) reason)) {
			tasks[i]->state &= ~TASK_STATE_WAITING;
			tasks[i]->data = 0;
		}
	}	
}
