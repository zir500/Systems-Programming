#ifndef _DEMOS_H_
#define _DEMOS_H_

#include "mutex.h"
#include "os.h"
#include "stdio.h"

void Mutex_Demo(void const *const args);
void FPU_demo(void const *const args);
void Wait_demo(void const *const args);
void Notify_Demo(void const *const args);
void demos_init(void);

#endif // _DEMOS_H_
