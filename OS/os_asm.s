    PRESERVE8
    AREA OS_func, CODE, READONLY

; Export function locations
    EXPORT SVC_Handler
    EXPORT PendSV_Handler
    EXPORT _task_switch
    EXPORT _task_init_switch

; Import global variables
    IMPORT _currentTCB
    IMPORT _OS_scheduler

; Import SVC routines
    IMPORT _svc_OS_enable_systick
    IMPORT _svc_OS_addTask
    IMPORT _svc_OS_task_exit
    IMPORT _svc_OS_yield
    IMPORT _svc_OS_schedule
	IMPORT _svc_OS_wait
	IMPORT _svc_OS_notify
	IMPORT _svc_OS_sleep
    
SVC_Handler
    ; Link register contains special 'exit handler mode' code
    ; Bit 2 tells whether the MSP or PSP was in use
    TST     lr, #4
    MRSEQ   r0, MSP
    MRSNE   r0, PSP
    ; r0 now contains the SP that was in use
    ; Return address is on the stack: load it into r1
    LDR     r1, [r0, #24]
    ; Use the return address to find the SVC instruction
    ; SVC instruction contains an 8-bit code
    LDRB    r1, [r1, #-2]
    ; Check if it's in the table
    CMP     r1, #((SVC_tableEnd - SVC_tableStart)/4)
    ; If not, return
    BXGE    lr
    ; Branch to the right handler
    ; Remember, the SP is in r0
    LDR     r2, =SVC_tableStart
    LDR     pc, [r2, r1, lsl #2]
    
    ALIGN
SVC_tableStart
    DCD _svc_OS_enable_systick
    DCD _svc_OS_addTask
    DCD _svc_OS_task_exit
    DCD _svc_OS_yield
    DCD _svc_OS_schedule
	DCD	_svc_OS_wait
	DCD	_svc_OS_notify
	DCD _svc_OS_sleep
SVC_tableEnd

    ALIGN
PendSV_Handler
	; Call the scheduler.
    STMFD   sp!, {r4, lr} ; r4 included for stack alignment
    LDR     r0, =_OS_scheduler
    BLX     r0
    LDMFD   sp!, {r4, lr}
_task_switch
    ; r0 contains nextTCB (OS_TCB *)
    ; Load r2 = &_currentTCB (OS_TCB **), r1 = _currentTCB (OS_TCB *, == OS_StackFrame **)
    LDR     r2, =_currentTCB
    LDR     r1, [r2]
    ; Compare _currentTCB to nextTCB: if equal, go home
    CMP     r1, r0
    BXEQ    lr
    ; If not, stack remaining process registers (pc, PSR, lr, r0-r3, r12 already stacked)
    MRS     r3, PSP
    STMFD   r3!, {r4-r11}
	; Fetch the FPCA bit from the control register.
	; r0 - &NewTCB
	; r1 - &_currentTCB
	; r2 - &&_currentTCB
	; r3 - OldTCB's stack Pointer
	; r12 - The Contents of the CONTROL Register af the end of OLD Task
	
	; Check whether the previous context was using the FPU by checking bit 4 of lr 
	; (Which was populated with EXC_RETURN on entry to this handler) 0 means the FPU was in use in the previous context
	ANDS r12, lr, #0x00000010
	BNE after_stacking_fpu
	;  The FPU bit was 0 so the FPU was in use.  Stack the remaining FPU Context (s15-s31)
	VSTMFD r3!, {s15-s31} ; Also will trigger lazy stacking to actually stack s0-s14, fpscr
after_stacking_fpu
	; Stack the value of EXC_RETURN so that we can work out whether a task uses the FPU when we switch back to it later.
	STMFD r3!, {r1, r12} ; r12 for alignment and convinience
    ; Store the stack pointer in the TCB
    STR     r3, [r1]
	
	
    ; Load new stack pointer
    LDR     r3, [r0]
	; Unstack the value of EXC_RETURN 
	LDMFD r3!, {r1, r12}
	ORR lr, #0x10 ; Assume the FPU wont be used and set EXC_RETURN[4] high.
	; Check if FPU was in use by this new task
	CMP r12, #0
	BNE after_popping_fpu
	; FPU was in use, pop the fpu registers & Reset the EXC_RETURN Bit
	BIC lr, #0x10
	VLDMFD r3!, {s15-s31}
after_popping_fpu
	; Set EXC_RETURN[4] to reflect whether the FPU is in use in this new context.
	

    ; Unstack process registers
    LDMFD   r3!, {r4-r11}
    MSR     PSP, r3
	ISB

	; Update _currentTCB
    STR     r0, [r2]
    ; Clear exclusive access flag
    CLREX
    BX      lr

    ALIGN
_task_init_switch
    ; Assume thread mode on entry
    ; Initial task is the idle task
    ; On entry r0 = OS_idleTCB_p (OS_TCB *)
    ; Load r1 = *(r0) (OS_StackFrame *)
    LDR     r1, [r0]
    ; Update PSP
    MSR     PSP, r1
    ; Update _currentTCB
    LDR     r2, =_currentTCB
    STR     r0, [r2]
    ; Switch to using PSP instead of MSP for thread mode (bit 1 = 1)
    ; Also lose privileges in thread mode (bit 0 = 1) and disable FPU (In this context) (bit 2 = 0)
    MOV     r2, #3
    MSR     CONTROL, r2
    ; Instruction barrier (stack pointer switch)
    ISB
	; Check to see if the scheduler is preemptive before
    ; This SVC call should be handled by _svc_OS_enable_systick()
    SVC     0x00
    ; Continue to the idle task
    
    ALIGN
    ; This SVC call should be handled by _svc_OS_schedule()
    ; It causes a switch to a runnable task, if possible
    SVC     0x04
_idle_task
    ; The following line is commented out because it doesn't play nicely with the debugger.
    ; For deployment, uncomment this line and the CPU will sleep when idling, waking only to
    ; handle interrupts.
;   WFI
    B       _idle_task
    
    ALIGN
    END
