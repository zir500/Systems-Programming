#include "mutex.h"
#include "os.h"
#include "stm32f4xx.h"                 


void OS_mutex_init(OS_mutex_t * const mutex){
	mutex->counter = 0;
	mutex->owner = 0;
}

void OS_mutex_aquire(OS_mutex_t * const mutex){
	while (1){
		uint32_t const mutex_owner = __LDREXW ((uint32_t*)mutex);
		uint32_t checkCode = OS_checkCode();
		
		if (mutex_owner == 0){
			if (__STREXW ((uint32_t)OS_currentTCB(), (uint32_t*)mutex)){
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
			OS_wait(mutex, checkCode);
		}
	}
}

void OS_mutex_release(OS_mutex_t * const mutex){
	if (mutex->owner == OS_currentTCB()){
		mutex->counter--;
		if (mutex->counter == 0){
			mutex->owner = 0;
			OS_notify(mutex);
		}
	}
}
