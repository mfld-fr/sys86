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
	push    %ds
	push    %ax
	push    %si
	push    %di

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

	mov     task_stack(%si),%di
	add     task_ssize(%si),%di
	sub     $REGS_KERN,%di

	pop     regs_di(%di)
	pop     regs_si(%di)
	pop     regs_ax(%di)
	pop     regs_ds(%di)
	pop     regs_sp(%di)
	pop     regs_ss(%di)

	mov     2*1(%bp),%bp  // vect num *

	lea     regs_di(%di),%sp
	mov     %ds,%ax
	mov     %ax,%ss

	// Push remaining unsaved registers

1:	push    %es
	push    %bx
	push    %cx
	push    %dx

	sti

	mov     %ds,%ax
	mov     %ax,%es

	// Call interrupt procedure

	pushw   %cs:(%bp)
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

1:	pop     %dx
	pop     %cx
	pop     %bx
	pop     %es
	pop     %di
	pop     %si
	pop     %ax
	pop     %ds
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
	push    %ds
	push    %ax
	push    %si
	push    %di
	push    %es
	push    %bx
	push    %cx
	push    %dx

	// Rearrange IP CS FL

	mov     2*2(%bp),%ax  // pushed FL
	xchg    %ax,2*4(%bp)  // exchange with pushed IP
	mov     %ax,2*2(%bp)  // right IP

	// Now jump to interrupt exit

	cli

	jmp     int_exit

//------------------------------------------------------------------------------

	.global stack_init_far

// Setup far stack with task initial context
// Tiny or small memory model SS=DS=ES

//  +4: arg1: task *
//  +6: arg2: task IP (user)
//  +8: arg3: task CS (user)
// +10: arg4: task SP (user)
// +12: arg5: task SS (user)
// +14: arg6: task SP (kernel)

stack_init_far:

	push    %bp
	mov     %sp,%bp

	mov     4(%bp),%bx  // arg1: task *

	push    %di
	mov     14(%bp),%di  // arg6: task SP (kernel)
	sub     $REGS_KERN,%di  // room on kernel stack
	mov     %di,TOP_SP(%bx)

	// Kernel stack

	cld
	xor     %ax,%ax
	stosw   // DX
	stosw   // CX
	stosw   // BX
	mov     12(%bp),%ax  // arg5: task SS (user)
	mov     %ax,%dx
	stosw   // ES
	xor     %ax,%ax
	stosw   // DI
	stosw   // SI
	stosw   // AX
	mov     %dx,%ax
	stosw   // DS
	mov     10(%bp),%ax  // arg4: task SP (user)
	sub     $REGS_USER,%ax  // room on user stack
	mov     %ax,%cx
	stosw   // SP
	mov     %dx,%ax
	stosw   // SS

	// User stack

	mov     %cx,%di
	push    %es
	mov     %dx,%es

	xor     %ax,%ax
	stosw   // BP
	stosw   // vect num *
	mov     6(%bp),%ax  // arg2: task IP (user)
	stosw   // IP
	mov     8(%bp),%ax  // arg3: task CS (user)
	stosw   // CS
	mov     $0x200,%ax  // STI
	stosw   // FL

	pop     %es
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

	mov     4(%bp),%bx  // arg1: task *

	push    %di
	mov     8(%bp),%di  // arg3: task SP (kernel)
	sub     $REGS_SIZE,%di
	mov     %di,TOP_SP(%bx)

	cld
	xor     %ax,%ax
	stosw   // DX
	stosw   // CX
	stosw   // BX
	mov     %ds,%ax
	stosw   // ES
	xor     %ax,%ax
	stosw   // DI
	stosw   // SI
	stosw   // AX
	mov     %ds,%ax
	stosw   // DS
	mov     %di,%ax
	add     $2*2,%ax
	stosw   // old SP
	mov     %ss,%ax
	stosw   // SS

	xor     %ax,%ax
	stosw   // BP
	stosw   // vect num *
	mov     6(%bp),%ax  // arg2: task IP (kernel)
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
