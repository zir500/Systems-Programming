#ifndef _DEMOS_H_
#define _DEMOS_H_

#include "mutex.h"
#include "os.h"
#include "stdio.h"

void mutexDemo(void const *const args);
void FPUDemo(void const *const args);
void waitDemo(void const *const args);
void notifyDemo(void const *const args);
void demosInit(void);

#endif // _DEMOS_H_
