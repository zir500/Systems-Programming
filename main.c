#include "os.h"
#include <stdio.h>
#include "utils/serial.h"
#include "fixedPriorityScheduler.h"
#include "demo.h"


/* MAIN FUNCTION */

int main(void) {
	/* Initialise the serial port so printf() works */
	serial_init();

	printf("\r\n~~~~~~DocetOS~~~~~~\r\n");

	/* Reserve memory for two stacks and two TCBs.
	   Remember that stacks must be 8-byte aligned. */
	__align(8)
	static uint32_t mutexDemoStack1[4096], 
									mutexDemoStack2[4096], 
									FPUDemoStack1[128],
									FPUDemoStack2[128],
									waitDemoStack[128],
									nofityDemoStack[128];
	
	static OS_TCB_t mutexDemo1, 
									mutexDemo2, 
									FPUDemo1,
									FPUDemo2,
									waitDemo,
									notifyDemo;
	
	OS_initialiseTCB(&mutexDemo1,mutexDemoStack1+4096, Mutex_Demo, FIXED_PRIORITY_HIGH, NULL);
	OS_initialiseTCB(&mutexDemo2,mutexDemoStack2+4096, Mutex_Demo, FIXED_PRIORITY_HIGH, NULL);
	
	OS_initialiseTCB(&FPUDemo1,FPUDemoStack1+128, FPU_demo, FIXED_PRIORITY_NORMAL, NULL);
	OS_initialiseTCB(&FPUDemo2,FPUDemoStack2+128, FPU_demo, FIXED_PRIORITY_NORMAL, NULL);
	
	OS_initialiseTCB(&waitDemo,		waitDemoStack+128, 	Wait_demo, FIXED_PRIORITY_HIGHEST, NULL);
	OS_initialiseTCB(&notifyDemo,	nofityDemoStack+128,	Notify_Demo, FIXED_PRIORITY_LOWEST, &waitDemo);

	OS_init(&fixedPriorityScheduler);

	demos_init();
	
	OS_addTask(&mutexDemo1);
	OS_addTask(&mutexDemo2);
	OS_addTask(&FPUDemo1);
	OS_addTask(&FPUDemo2);
	OS_addTask(&waitDemo);
	OS_addTask(&notifyDemo);
	
	OS_start();
}
