// SYS86 project
// Architecture specific interrupt handling

#define _ASSEMBLY

#include "task.h"
#include "int.h"

	.code16

	.text

//------------------------------------------------------------------------------

	.global vect_init

vect_init:

	push    %es
	xor     %ax,%ax
	mov     %ax,%es

	cli

	// Timer vector

	mov     $4*VECT_TIMER0,%bx
	movw    $int_timer0,%es:(%bx)
	mov     %cs,%es:2(%bx)

	// Serial vector

	mov     $4*VECT_SERIAL,%bx
	movw    $int_serial,%es:(%bx)
	mov     %cs,%es:2(%bx)

	sti

	// System vector

	mov     $4*VECT_SYSTEM,%bx
	movw    $int_sys,%es:(%bx)
	mov     %cs,%es:2(%bx)

	pop     %es
	ret

//------------------------------------------------------------------------------

// Timer interrupt

	.global int_timer0

int_timer0:

#ifdef CONFIG_TRACE
	pushw   $0x10
	call    trace_far
	add     $2,%sp
#endif // CONFIG_TRACE

	pushw   $VECT_TIMER0
	jmp     int_hand

// Serial interrupt

	.global int_serial

int_serial:

#ifdef CONFIG_TRACE
	pushw   $0x20
	call    trace_far
	add     $2,%sp
#endif // CONFIG_TRACE

	pushw   $VECT_SERIAL
	jmp     int_hand

//------------------------------------------------------------------------------

// Asynchronous interrupt (HW IRQ or trap)

int_hand:

	// Keep interrupt disabled
	// until context fully saved
	// and stack switch completed

	// IP CS FL already pushed
	// Push remaining registers

	push    %es
	push    %ds

	push    %ax
	push    %cx
	push    %dx
	push    %bx
	push    %bp
	push    %si
	push    %di

	// Get interrupt number

	mov     %sp,%bp
	mov     9*2(%bp),%cx

	// Switch to kernel data segment

	mov     $KERNEL_DS,%dx
	mov     %dx,%ds

	// Get current task

	mov     task_cur,%si

	// Increment & check interrupt level
	// =1: first interrupt
	// >1: nested interrupt

	mov     int_level,%ax
	inc     %ax
	mov     %ax,int_level
	cmp     $1,%ax
	jne     1f

	// Save interrupted stack top
	// Switch to interrupt stack

	mov     %sp,task_tsp(%si)
	mov     %ss,task_tss(%si)

	mov     int_stack,%sp
	add     $INT_STACK_SIZE,%sp
	mov     %dx,%ss

1:	sti

	// Call interrupt procedure
	// with interrupt number

	push    %cx
	call    int_proc
	add     $2,%sp

	// Keep interrupt disabled
	// until all stack switches completed
	// and context fully restored

	cli

	// Decrement & check interrupt level
	// =0: back to normal
	// >0: still interrupted

	mov     int_level,%ax
	dec     %ax
	mov     %ax,int_level
	jnz     int_ret

	// Switch back to interrupted stack

	//mov     task_cur,%si
	mov     task_tsp(%si),%sp
	mov     task_tss(%si),%ss

	mov     $1,%di  // interrupted

	// Now the interrupt stack is empty
	// and context on top of interrupted stack
	// so task switching can occur

stack_switch:

	// May save previous stack top

	mov     task_prev,%bx
	or      %bx,%bx
	jz      1f

	mov     %di,task_int(%bx)

	mov     %sp,task_tsp(%bx)
	mov     %ss,task_tss(%bx)

	xor     %bx,%bx
	mov     %bx,task_prev

	// May switch to next stack top

1:	mov     task_next,%bx
	or      %bx,%bx
	jz      1f

	mov     task_tsp(%bx),%sp
	mov     task_tss(%bx),%ss

	mov     task_int(%bx),%di

	mov     %bx,%si
	mov     %si,task_cur

	xor     %bx,%bx
	mov     %bx,task_next

	// Check task interrupt status

1:	or      %di,%di
	jz      switch_ret
	movw    $0,task_int(%si)

int_ret:

	// Restore registers from interrupted stack

	pop     %di
	pop     %si
	pop     %bp
	pop     %bx
	pop     %dx
	pop     %cx
	pop     %ax

	pop     %ds
	pop     %es

	add     $2,%sp  // skip int_num

	// Interrupt return for remaining IP CS FL

#ifdef CONFIG_TRACE
	pushw   $0x30
	call    trace_far
	add     $2,%sp
#endif // CONFIG_TRACE

	iret

//------------------------------------------------------------------------------

	.global task_switch

// Switch task

task_switch:

	// Save task interrupt flag
	// and registers required by GCC-IA16

	pushf
	push    %es
	push    %bp
	push    %si
	push    %di

	xor     %di,%di  // not interrupted
	jmp     stack_switch

switch_ret:

	pop     %di
	pop     %si
	pop     %bp
	pop     %es
	popf

#ifdef CONFIG_TRACE
	pushw   $0x40
	call    trace_far
	add     $2,%sp
#endif // CONFIG_TRACE

	ret

//------------------------------------------------------------------------------

// Synchronous interrupt (system call)

	.global int_sys

int_sys:

	push    %ds
	push    %si
	push    %di

	// Switch to kernel data segment

	mov     $KERNEL_DS,%di
	mov     %di,%ds

	// Get current task

	mov     task_cur,%si

	// Save user stack top
	// Switch to kernel stack

	mov     %sp,task_usp(%si)
	mov     %ss,task_uss(%si)

	mov     task_stack(%si),%sp
	add     task_ssize(%si),%sp
	mov     %di,%ss
	sti

	// Call system procedure

	// TODO: push caller context
	// call sys_proc
	// add $xxx,%sp

	// Switch back to user stack

	cli
	mov     task_usp(%si),%sp
	mov     task_uss(%si),%ss

	pop     %di
	pop     %si
	pop     %ds
	iret

//------------------------------------------------------------------------------

	.global stack_init_far

// Setup far stack with task initial context
// Tiny or small memory model SS=DS=ES

//  +4: arg1: task *
//  +6: arg2: task IP (user)
//  +8: arg3: task CS (user)
// +10: arg4: task SP (user)
// +12: arg5: task SS (user)

stack_init_far:

	push    %bp
	mov     %sp,%bp

	mov     4(%bp),%bx  // arg1: task *

	push    %di
	mov     10(%bp),%di  // arg4
	mov     12(%bp),%dx  // arg5
	sub     $REGS_SIZE,%di

	push    %si
	mov     %di,%si

	push    %es
	mov     %dx,%es

	cld
	xor     %ax,%ax
	mov     $7,%cx  // 7 general registers
	rep
	stosw   // DI SI BP BX DX CX AX
	mov     %dx,%ax
	stosw   // DS
	stosw   // ES
	xor     %ax,%ax
	stosw   // int num
	mov     6(%bp),%ax  // arg2
	stosw   // IP
	mov     8(%bp),%ax  // arg3
	stosw   // CS
	mov     $0x200,%ax  // STI
	stosw   // FL

	pop     %es

	// Save stack top in task

	mov     %si,task_tsp(%bx)
	mov     %dx,task_tss(%bx)

	pop     %si
	pop     %di
	pop     %bp
	ret

//------------------------------------------------------------------------------

	.global stack_init_near

// Setup near stack with task initial context
// Tiny or small memory model SS=DS=ES

// +4: arg1: task *
// +6: arg2: task IP (kernel)
// +8: arg3: task SP (kernel)

stack_init_near:

	push    %bp
	mov     %sp,%bp

	mov     8(%bp),%bx  // arg3
	sub     $6*2,%bx

	xor     %ax,%ax
	mov     %ax,0(%bx)  // DI
	mov     %ax,2(%bx)  // SI
	mov     %ax,4(%bx)  // BP
	mov     %ds,6(%bx)  // ES

	movw    $0x200,8(%bx)  // FL STI

	mov     6(%bp),%ax  // arg2
	mov     %ax,10(%bx)  // IP

	// Save stack top in task

	mov     %bx,%ax
	mov     4(%bp),%bx  // arg1
	mov     %ax,task_tsp(%bx)

	mov     %ds,%ax
	mov     %ax,task_tss(%bx)

	pop     %bp
	ret

//------------------------------------------------------------------------------
/*
	.global int_enable

int_enable:

	sti
	ret
*/
//------------------------------------------------------------------------------

	.global int_save

// Save interrupt enabled flag then disable it

int_save:

	pushf
	pop     %ax
	and     $0x0200,%ax
	cli
	ret

//------------------------------------------------------------------------------

	.global int_back

// Back to saved interrupt enable flag

// arg1: flag

int_back:

	mov     %sp,%bx
	mov     2(%bx),%ax  // arg1
	pushf
	pop     %dx
	and     $0xFEFF,%dx
	or      %ax,%dx
	push    %dx
	popf
	ret

//------------------------------------------------------------------------------
