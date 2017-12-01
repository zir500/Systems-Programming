#include "os.h"
#include <stdio.h>
#include "utils/serial.h"
#include "fixedPriorityScheduler.h"
#include "mutex.h"

static OS_mutex_t mutex;

void task1(void const *const args) {
	for (int i=0; i < 10; i++)  {
		printf("Message from Task 1a\r\n");
	}
	OS_sleep(OS_elapsedTicks() + 1000);
	for (int i=0; i < 10; i++)  {
		printf("Message from Task 1b\r\n");
	}
}
void task2(void const *const args) {
	for (int i=0; i < 10; i++) {
		printf("Message from Task 2\r\n");
	}
}

void task3(void const *const args) {
	for (int i=0; i < 10; i++) {
		printf("Message from Task 3\r\n");
	}
}

/* MAIN FUNCTION */

int main(void) {
	/* Initialise the serial port so printf() works */
	serial_init();

	printf("\r\nDocetOS Sleep and Mutex\r\n");

	/* Reserve memory for two stacks and two TCBs.
	   Remember that stacks must be 8-byte aligned. */
	__align(8)
	static uint32_t stack1[64], stack2[64], stack3[64];
	static OS_TCB_t TCB1, TCB2, TCB3;

	OS_initialiseTCB(&TCB1, stack1+64, task1, FIXED_PRIORITY_HIGHEST, 0);
	OS_initialiseTCB(&TCB2, stack2+64, task2, FIXED_PRIORITY_NORMAL, 0);
	OS_initialiseTCB(&TCB3, stack3+64, task3, FIXED_PRIORITY_LOWEST, 0);


	OS_init(&fixedPriorityScheduler);
	
	OS_mutex_init(&mutex);
	
	OS_addTask(&TCB1);
	OS_addTask(&TCB2);
	OS_addTask(&TCB3);
	
	OS_start();
}
