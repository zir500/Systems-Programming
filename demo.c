#include "demo.h"

static OS_mutex_t print_mutex;
static OS_mutex_t ackermann_mutex;

/* Prints a string to the terminal (Just a convinience function) */
void prints(char * const str){
	OS_mutexAquire(&print_mutex);
	printf("%s", str);
	OS_mutexRelease(&print_mutex);
}

/* This uses teh FPU to calculate an ackermann function.
 * It is highly recursive and uses a mutex to prevent any other task from calling this function at the same time
 * (There isn't any reason to prevent other tasks from calling it except to demo the recursive mutex.)
 */
float ackermann(float m, float n){
	OS_mutexAquire(&ackermann_mutex);
	float A_m_n = 0;
	if (m == 0.0f) {
		A_m_n = n + 1;
		OS_mutexRelease(&ackermann_mutex);
		return A_m_n;
	} 
	if (n == 0.0f) {
		A_m_n = ackermann(m - 1.0f, 1.0f);
		OS_mutexRelease(&ackermann_mutex);
		return A_m_n;
	}
	A_m_n = ackermann(m - 1.0f, ackermann(m, n - 1.0f));
	OS_mutexRelease(&ackermann_mutex);
	return A_m_n;
}

/* Uses the above ackerman function to demonstrate the recursive mutex works, 
 * and that prevents other tasks from claiming it. 
*/
void Mutex_Demo(void const *const args){
	prints("Starting Mutex Demo\r\n");
	OS_sleep(2); // Wait for the other Mutex_demo to start
	float A = ackermann(2.0, 2.0);
	
	// Print the value of A(2,2)
	OS_mutexAquire(&print_mutex);
	printf("A(2, 2) = %.1f\r\n", A);
	OS_mutexRelease(&print_mutex);
	
	prints("Mutex Demo Finished\r\n");
}



/* This task does a few operations on the FPU, slees for a period of time - allowing a context switch to occur
 * then resumes the calculations from where it left off before it went to sleep
*/
void FPU_demo(void const *const args){
	float ten = 10.0f;
	float four = 4.0f;
	float five = 5.0f;
	float ten_ovr_four = ten/four;
	OS_mutexAquire(&print_mutex);
	printf("10/4 = %.2f\r\n", ten_ovr_four);
	OS_mutexRelease(&print_mutex);
	
	prints("FPU Demo Sleeping\r\n");
	OS_sleep(10);
	
	OS_mutexAquire(&print_mutex);
	printf("(10/4) * 5 = %.2f\r\n", ten_ovr_four*five);
	OS_mutexRelease(&print_mutex);
}

/* This task demonstrates Wait by waiting as soon as it begins.
 * Some other task will notify this task after a period of time.
*/
void Wait_demo(void const *const args){
	prints("Wait Demo Running.  Waiting...\r\n");
	OS_wait(OS_checkCode());
	prints("Wait demo notified.\r\n");
}

/* This task will sleep for approximately a second then will notify the Wait_demo
 * The TCB of Wait_demo is passed into this task through the arguments.
*/
void Notify_Demo(void const *const args){
	prints("Notify Demo Running.\r\n");
	OS_sleep(1000);
	OS_notify((OS_TCB_t*) args);
}


/* Does any setup required for the demos to run */
void demos_init(){
	OS_mutexInit(&print_mutex);
	OS_mutexInit(&ackermann_mutex);
}
