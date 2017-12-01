#include "sleep.h"

#include "OS.h"

void OS_sleep(uint32_t sleep_ticks){
	OS_TCB_t* currentTask = OS_currentTCB();
	uint32_t currentTicks = OS_elapsedTicks();
	
	currentTask->state = TASK_STATE_SLEEP;
	currentTask->data = currentTicks + sleep_ticks; 
	OS_yield();
}
