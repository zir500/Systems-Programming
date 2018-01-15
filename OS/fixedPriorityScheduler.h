#ifndef _FIXED_PRIORITY_SCHEDULER_H_
#define _FIXED_PRIORITY_SCHEDULER_H_

#include "os.h"

/* Returns true if tick a is later than tick b.  The difference between tick a and b must be less than 
   the maximum positive value of a signed 32bit int */
#define IS_AFTER(a, b) (int32_t)(a) - (int32_t)(b) > 0

enum OS_FPSCHEDULER_PRIORITY_e {
	FIXED_PRIORITY_HIGHEST = 0,
	FIXED_PRIORITY_HIGH,
	FIXED_PRIORITY_NORMAL,
	FIXED_PRIORITY_LOW,
	FIXED_PRIORITY_LOWEST
};

extern OS_Scheduler_t const fixedPriorityScheduler;



#endif // _FIXED_PRIORITY_SCHEDULER_H_
