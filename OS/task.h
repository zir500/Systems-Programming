#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include <stddef.h>

/* Describes a single stack frame, as found at the top of the stack of a task
   that is not currently running.  Registers r0-r3, r12, lr, pc and psr are stacked
	 automatically by the CPU on entry to handler mode.  Registers r4-r11 are subsequently
	 stacked by the task switcher.  That's why the order is a bit weird. */
typedef struct s_StackFrame {
	volatile uint32_t spacer;		 
	volatile uint32_t isFpuNotInUse; //  When context switch occurs, this is 0 if the FPU Wasn't in use 
	volatile uint32_t r4;
	volatile uint32_t r5;
	volatile uint32_t r6;
	volatile uint32_t r7;
	volatile uint32_t r8;
	volatile uint32_t r9;
	volatile uint32_t r10;
	volatile uint32_t r11;
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr;
	volatile uint32_t pc;
	volatile uint32_t psr;
} OS_StackFrame_t;

typedef struct s_FloatingPointStackFrame {
	volatile uint32_t isFpuNotInUse;
	volatile uint32_t EXC_RETURN;
	//Callee Stacked FPU Registers
	volatile uint32_t s15;
	volatile uint32_t s16;
	volatile uint32_t s17;
	volatile uint32_t s18;
	volatile uint32_t s19;
	volatile uint32_t s20;
	volatile uint32_t s21;
	volatile uint32_t s22;
	volatile uint32_t s23;
	volatile uint32_t s24;
	volatile uint32_t s25;
	volatile uint32_t s26;
	volatile uint32_t s27;
	volatile uint32_t s28;
	volatile uint32_t s29;
	volatile uint32_t s30;
	volatile uint32_t s31;
	//Callee Stacked CPU Registers
	volatile uint32_t r4;
	volatile uint32_t r5;
	volatile uint32_t r6;
	volatile uint32_t r7;
	volatile uint32_t r8;
	volatile uint32_t r9;
	volatile uint32_t r10;
	volatile uint32_t r11;
	// Caller Stacked CPU Registers
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr;
	volatile uint32_t pc;
	volatile uint32_t psr;
	// Caller Stacked FPU Registers
	volatile uint32_t 	s0;
	volatile uint32_t 	s1;
	volatile uint32_t 	s2;
	volatile uint32_t 	s3;
	volatile uint32_t 	s4;
	volatile uint32_t 	s5;
	volatile uint32_t 	s6;
	volatile uint32_t 	s7;
	volatile uint32_t 	s8;
	volatile uint32_t 	s9;
	volatile uint32_t 	s10;
	volatile uint32_t 	s11;
	volatile uint32_t 	s12;
	volatile uint32_t 	s13;
	volatile uint32_t 	s14;
	volatile uint32_t  FPSCR;
	volatile uint32_t spacer;
} FloatingPointStackFrame_t;

typedef struct OS_TCB {
	/* Task stack pointer.  It's important that this is the first entry in the structure,
	   so that a simple double-dereference of a TCB pointer yields a stack pointer. */
	void * volatile sp;
	/* The remaining fields are provided for expandability.  None of them have a dedicated
	   purpose, but their names might imply a use.  Feel free to use these fields for anything
	   you like. */
	uint32_t volatile priority;
	uint32_t volatile wakeupTime;
	
	/* For scheduling, Pointer to the next in the queue to be executed and to the previous task in the queue */
	struct OS_TCB  * volatile next; 
	struct OS_TCB  * volatile prev;
} OS_TCB_t;


/* Contains the head and tail elements for a linked lisk of TCBs */
typedef struct {
	OS_TCB_t * volatile head;
	OS_TCB_t * volatile tail;
} OS_TaskList_t;

/*  */
void OS_initialiseList(OS_TaskList_t * const list);

/* Adds the given TCB to the end of a list */
void OS_appendToList(OS_TaskList_t * const list, OS_TCB_t * const task);

/* Inserts the given task into a list which is sorted by descending levels or priority 
 * ie, the tcb at the head has the highest priority
*/
void OS_addToListByPriority(OS_TaskList_t * const list, OS_TCB_t * const task); 

/* Removes the given TCB from a list. */
void OS_removeFromList(OS_TaskList_t * const list, OS_TCB_t * const task);


#endif /* _TASK_H_ */
