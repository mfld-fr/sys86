// SYS86 project
// Architecture specific interrupt handling

#define _ASSEMBLY

#include "task.h"
#include "int.h"

#include "sample.S"

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

	/*
	mov     $4*VECT_SYSTEM,%bx
	movw    $int_system,%es:(%bx)
	mov     %cs,%es:2(%bx)
	*/

	// Global vector

	mov     $4*VECT_GLOBAL,%bx
	mov     %ax,%es:(%bx)
	mov     %ds,%es:2(%bx)

	pop     %es
	ret

//------------------------------------------------------------------------------

// Timer interrupt

	.global int_timer0

int_timer0:

	call    int_hand
	.word   VECT_TIMER0

// Serial interrupt

	.global int_serial

int_serial:

	time_sample 0

	call    int_hand
	.word   VECT_SERIAL

// System call (synchronous)

/*
	.global int_system

int_system:

	call    int_hand
	.word   VECT_SYSTEM
*/

//------------------------------------------------------------------------------

int_hand:

	// Keep interrupt disabled
	// until context fully saved
	// and stack switch completed

	// IP CS FL already pushed
	// Get some registers available

	push    %bp
	mov     %sp,%bp
	push    %ss
	push    %bp  // actually old SP
	push    %es
	push    %ds
	push    %ax
	push    %si

	// Switch to kernel data segment

	xor     %ax,%ax
	mov     %ax,%ds
	mov     4*VECT_GLOBAL+2,%ax
	mov     %ax,%ds

	// Increment & check task level
	// =1: was in user mode
	// >1: was in kernel mode

	mov     task_cur,%si
	mov     task_level(%si),%ax
	inc     %ax
	mov     %ax,task_level(%si)
	cmp     $1,%ax
	jne     1f

	// Was on user stack
	// Switch to kernel stack

	mov     task_stack(%si),%sp
	add     task_ssize(%si),%sp
	mov     %ss,%ax
	mov     %ax,%es
	mov     %ds,%ax
	mov     %ax,%ss

	push    %es:-2*1(%bp)  // old SS
	push    %es:-2*2(%bp)  // old SP
	push    %es:-2*3(%bp)  // ES
	push    %es:-2*4(%bp)  // DS
	push    %es:-2*5(%bp)  // AX
	push    %es:-2*6(%bp)  // SI

	// Push remaining unsaved registers

1:	push    %bx
	push    %cx
	push    %dx
	push    %di

	// Get interrupt vector number

	sti

	mov     %es:2*1(%bp),%bx  // vect num *
	mov     %cs:(%bx),%dx
	mov     %ds,%ax
	mov     %ax,%es

	// Call interrupt procedure

	push    %dx
	call    int_proc
	add     $2,%sp

	// Keep interrupt disabled
	// until all stack switches completed
	// and context fully restored

	cli

	// Decrement task level
	// =0: back to user mode
	// >0: still in kernel mode

	//mov     task_cur,%si
	decw    task_level(%si)

	// Now the interrupt stack is empty
	// and context on top of interrupted stack
	// so task switching can occur

int_exit:

	// May save previous stack top

	mov     task_prev,%bx
	or      %bx,%bx
	jz      1f

	mov     %sp,TOP_SP(%bx)

	xor     %bx,%bx
	mov     %bx,task_prev

	// May switch to next stack top

1:	mov     task_next,%bx
	or      %bx,%bx
	jz      1f

	mov     TOP_SP(%bx),%sp

	mov     %bx,task_cur
	xor     %bx,%bx
	mov     %bx,task_next

	// Restore registers from stack

1:	pop     %di
	pop     %dx
	pop     %cx
	pop     %bx
	pop     %si
	pop     %ax
	pop     %ds
	pop     %es
	pop     %bp  // actually new SP
	pop     %ss
	mov     %bp,%sp
	pop     %bp

	add     $2,%sp  // skip vect num *

	// Interrupt return for remaining IP CS FL

	time_sample 2

	iret

//------------------------------------------------------------------------------

	.global task_switch

// Switch task out of interrupt

// Push context as for interrupt entry
// then jump to interrupt exit

task_switch:

	// Caller IP already pushed
	// Bad order for FL CS IP
	// Will rearrange later

	push    %cs
	pushf

	// Push other registers

	sub     $2,%sp  // skip vect num *

	push    %bp
	mov     %sp,%bp
	push    %ss
	push    %bp

	push    %es
	push    %ds

	push    %ax
	push    %si
	push    %bx
	push    %cx
	push    %dx
	push    %di

	// Rearrange to IP CS FL

	mov     2*2(%bp),%ax  // pushed FL
	xchg    %ax,2*4(%bp)  // exchange with pushed IP
	mov     %ax,2*2(%bp)  // right IP

	// Now jump to interrupt exit

	cli

	jmp     int_exit

//------------------------------------------------------------------------------
/*
TODO: rework for kernel / user stack chain

	.global stack_init_far

// Setup far stack with task initial context
// Tiny or small memory model SS=DS=ES

//  +4: arg1: task *
//  +6: arg2: task IP
//  +8: arg3: task CS
// +10: arg4: task SP
// +12: arg5: task SS

stack_init_far:

	push    %bp
	mov     %sp,%bp

	mov     4(%bp),%bx  // arg1

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
	mov     $8,%cx  // 8 general registers
	rep
	stosw   // DI SI BP dummy-SP BX DX CX AX
	mov     %dx,%ax
	stosw   // DS
	stosw   // ES
	xor     %ax,%ax
	stosw   // int num *
	mov     6(%bp),%ax  // arg2
	stosw   // IP
	mov     8(%bp),%ax  // arg3
	stosw   // CS
	mov     $0x200,%ax  // STI
	stosw   // FL

	pop     %es

	// Save stack top in task

	mov     %si,TOP_SP(%bx)
	mov     %dx,TOP_SS(%bx)

	pop     %si
	pop     %di
	pop     %bp
	ret
*/

//------------------------------------------------------------------------------

	.global stack_init_near

// Setup near stack with task initial context
// Tiny or small memory model SS=DS=ES

// +4: arg1: task *
// +6: arg2: task IP
// +8: arg3: task SP

stack_init_near:

	push    %bp
	mov     %sp,%bp

	mov     4(%bp),%bx  // arg1: task *

	push    %di
	mov     8(%bp),%di  // arg3: task SP
	sub     $REGS_SIZE,%di
	mov     %di,TOP_SP(%bx)

	cld
	xor     %ax,%ax
	mov     $6,%cx  // 6 general registers
	rep
	stosw   // DI DX CX BX SI AX
	mov     %ds,%ax
	stosw   // DS
	stosw   // ES
	mov     %di,%ax
	add     $2*2,%ax
	stosw   // old SP
	mov     %ss,%ax
	stosw   // SS
	xor     %ax,%ax
	stosw   // BP
	stosw   // vect num *
	mov     6(%bp),%ax  // arg2: task IP
	stosw   // IP
	mov     %cs,%ax
	stosw   // CS
	mov     $0x200,%ax  // STI
	stosw   // FL

	pop     %di
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
