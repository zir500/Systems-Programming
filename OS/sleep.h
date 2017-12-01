#ifndef SLEEP_H
#define SLEEP_H

#include <stdint.h>

/* Prevents the current task from running for a given number of prempt ticks.  
 * Sleep_ticks MUST not exceed 2^31.
 * NB, not accurate timing.  Worst case inaccuracy: (MAX_TASKS-1) * TICK_TIME ms
 */
void OS_sleep(uint32_t sleep_ticks);

#endif //SLEEP_H
