# Year 4 Systems Programming for ARM Assignment

This is my submission for a systems programming module.  The task was to expand upon the funtionality of the minimal OS base provided and to produce a report on the design and implementation of the OS.

I have implemented some additional features I feel would be useful to begin forming an OS suitable for embedded situations where a limited number of tasks would be necessary.  This lead to an improvement to the default task scheduling system.  A fixed-priority scheduler has been implemented to replace the default round-robin based scheduler.  The way tack control blocks (TCBs) are stored has been changed as part of this to minimise the complexity of a task switch at the cost of additioal complexity at the task startup stage.  

As part of the improved scheduling system, tasks may be put to sleep for a set perios of time, or set to sleep until notified by another task.

A recursive mutex has been added.  This is safe to use in privileged code.

The hardware FPU avaliable on the ARM Corex-M4 has been supported so that it can safely be used by multiple tasks simultaniously. 

Most of os.c os.h os.asm task.h was part of the minimal implementation and was written by Andy Pomfret. 
