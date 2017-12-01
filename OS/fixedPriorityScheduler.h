#ifndef FIXED_PRIORITY_SCHEDULER_H
#define FIXED_PRIORITY_SCHEDULER_H

#include "os.h"

/* Returns true if tick a is later than tick b.  The difference between tick a and b must be less than 
   the maximum positive value of a signed 32bit int */
#define IS_AFTER(a, b) (int32_t)(a) - (int32_t)(b) > 0

#define FIXED_PRIORITY_MAX_TASKS 8

enum OS_FPSCHEDULER_PRIORITY_e {
	FIXED_PRIORITY_HIGHEST = 0,
	FIXED_PRIORITY_HIGH,
	FIXED_PRIORITY_NORMAL,
	FIXED_PRIORITY_LOW,
	FIXED_PRIORITY_LOWEST
};

extern OS_Scheduler_t const fixedPriorityScheduler;



#endif // FIXED_PRIORITY_SCHEDULER_H
