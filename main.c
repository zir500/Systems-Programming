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
									waitDemoTCB,
									notifyDemoTCB;
	
	OS_initialiseTCB(&mutexDemo1,mutexDemoStack1+4096, mutexDemo, FIXED_PRIORITY_HIGH, NULL);
	OS_initialiseTCB(&mutexDemo2,mutexDemoStack2+4096, mutexDemo, FIXED_PRIORITY_HIGH, NULL);
	
	OS_initialiseTCB(&FPUDemo1,FPUDemoStack1+128, FPUDemo, FIXED_PRIORITY_NORMAL, NULL);
	OS_initialiseTCB(&FPUDemo2,FPUDemoStack2+128, FPUDemo, FIXED_PRIORITY_NORMAL, NULL);
	
	OS_initialiseTCB(&waitDemoTCB,		waitDemoStack+128, 	waitDemo, FIXED_PRIORITY_HIGHEST, NULL);
	OS_initialiseTCB(&notifyDemoTCB,	nofityDemoStack+128,	notifyDemo, FIXED_PRIORITY_LOWEST, &waitDemoTCB);

	OS_init(&fixedPriorityScheduler);

	demosInit();
	
	OS_addTask(&mutexDemo1);
	OS_addTask(&mutexDemo2);
	OS_addTask(&FPUDemo1);
	OS_addTask(&FPUDemo2);
	OS_addTask(&waitDemoTCB);
	OS_addTask(&notifyDemoTCB);
	
	OS_start();
}
