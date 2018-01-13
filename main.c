#include "os.h"
#include <stdio.h>
#include "utils/serial.h"
#include "fixedPriorityScheduler.h"
#include "mutex.h"

static OS_mutex_t print_mutex;

float ackermann(float m, float n){
	OS_mutex_aquire(&print_mutex);
	printf("A");
	OS_mutex_release(&print_mutex);
	OS_sleep(20);
	if (m == 0.0f) {
		return n + 1;
	} 
	if (n == 0.0f) {
		return ackermann(m - 1.0f, 1.0f);
	}
	return ackermann(m - 1.0f, ackermann(m, n - 1.0f));
}

void ackermann_sample(void const *const args) {
	// As an Example of proper management of the stack when using the FPU, 
	// Calculate some	ackermann function which usesthe fpu (for no good reason really)
	float val = ackermann(3.0f, 1.0f);
	OS_mutex_aquire(&print_mutex);
	printf("A(3, 3) = %.2f \r\n", val);
	OS_mutex_release(&print_mutex);
}

void task2(void const *const args) {
	for (int i=0; i <= 100; i++) {
		OS_mutex_aquire(&print_mutex);
		printf("Task 2\r\n");
		OS_mutex_release(&print_mutex);
		OS_sleep(10);
	}
}

void task3(void const *const args) {
	for (int i=0; i < 10; i++) {
		OS_mutex_aquire(&print_mutex);
		printf("Message from Task 3\r\n");
		OS_mutex_release(&print_mutex);
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
	static uint32_t stack1[4096] = {6}, stack2[64], stack3[64];
	static OS_TCB_t TCB1, TCB2, TCB3;

	OS_initialiseTCB(&TCB1, stack1+4096, ackermann_sample, FIXED_PRIORITY_HIGHEST, 0);
	OS_initialiseTCB(&TCB2, stack2+64, task2, FIXED_PRIORITY_NORMAL, 0);
	OS_initialiseTCB(&TCB3, stack3+64, task3, FIXED_PRIORITY_LOWEST, 0);


	OS_init(&fixedPriorityScheduler);
	
	OS_mutex_init(&print_mutex);
	
	OS_addTask(&TCB1);
	OS_addTask(&TCB2);
	//OS_addTask(&TCB3);
	
	OS_start();
}
