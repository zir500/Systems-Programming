#include "demo.h"

static OS_mutex_t printMutex;
static OS_mutex_t ackermannMutex;

/* Prints a string to the terminal (Just a convinience function) */
void prints(char * const str){
	OS_mutexAquire(&printMutex);
	printf("%s", str);
	OS_mutexRelease(&printMutex);
}

/* This uses teh FPU to calculate an ackermann function.
 * It is highly recursive and uses a mutex to prevent any other task from calling this function at the same time
 * (There isn't any reason to prevent other tasks from calling it except to demo the recursive mutex.)
 */
float ackermann(float m, float n){
	OS_mutexAquire(&ackermannMutex);
	float A_m_n = 0;
	if (m == 0.0f) {
		A_m_n = n + 1;
		OS_mutexRelease(&ackermannMutex);
		return A_m_n;
	} 
	if (n == 0.0f) {
		A_m_n = ackermann(m - 1.0f, 1.0f);
		OS_mutexRelease(&ackermannMutex);
		return A_m_n;
	}
	A_m_n = ackermann(m - 1.0f, ackermann(m, n - 1.0f));
	OS_mutexRelease(&ackermannMutex);
	return A_m_n;
}

/* Uses the above ackerman function to demonstrate the recursive mutex works, 
 * and that prevents other tasks from claiming it. 
*/
void mutexDemo(void const *const args){
	prints("Starting Mutex Demo\r\n");
	OS_sleep(2); // Wait for the other mutexDemo to start
	float A = ackermann(2.0, 2.0);
	
	// Print the value of A(2,2)
	OS_mutexAquire(&printMutex);
	printf("A(2, 2) = %.1f\r\n", A);
	OS_mutexRelease(&printMutex);
	
	prints("Mutex Demo Finished\r\n");
}



/* This task does a few operations on the FPU, slees for a period of time - allowing a context switch to occur
 * then resumes the calculations from where it left off before it went to sleep
*/
void FPUDemo(void const *const args){
	float ten = 10.0f;
	float four = 4.0f;
	float five = 5.0f;
	float ten_ovr_four = ten/four;
	OS_mutexAquire(&printMutex);
	printf("10/4 = %.2f\r\n", ten_ovr_four);
	OS_mutexRelease(&printMutex);
	
	prints("FPU Demo Sleeping\r\n");
	OS_sleep(10);
	
	OS_mutexAquire(&printMutex);
	printf("(10/4) * 5 = %.2f\r\n", ten_ovr_four*five);
	OS_mutexRelease(&printMutex);
}

/* This task demonstrates Wait by waiting as soon as it begins.
 * Some other task will notify this task after a period of time.
*/
void waitDemo(void const *const args){
	prints("Wait Demo Running.  Waiting...\r\n");
	OS_wait(OS_checkCode());
	prints("Wait demo notified.\r\n");
}

/* This task will sleep for approximately a second then will notify the waitDemo
 * The TCB of waitDemo is passed into this task through the arguments.
*/
void notifyDemo(void const *const args){
	prints("Notify Demo Running.\r\n");
	OS_sleep(1000);
	OS_notify((OS_TCB_t*) args);
}


/* Does any setup required for the demos to run */
void demosInit(){
	OS_mutexInit(&printMutex);
	OS_mutexInit(&ackermannMutex);
}
