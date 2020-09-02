
#define _ASSEMBLY

#include "task.h"
#include "int.h"

// Stack frame after task switch entry

#define switch_di     2*0x0
#define switch_si     2*0x1
#define switch_bp     2*0x2
#define switch_es     2*0x3
#define switch_ip     2*0x4

#define SWITCH_SIZE   2*0x5

// Timer interrupt vector

#define vect_timer 0x08

// System call vector with kernel CS

#define vect_system 0x80

// Global vector with kernel DS

#define vect_global 0xFF
#define kern_ds (4*vect_global+2)

//------------------------------------------------------------------------------

	.code16

	.text

//------------------------------------------------------------------------------

	.global vect_init

vect_init:

	push    %es
	xor     %ax,%ax
	mov     %ax,%es

	// Timer vector

	mov     $4*vect_timer,%bx
	movw    $int_timer,%es:(%bx)
	mov     %cs,%es:2(%bx)

	// System vector

	/*
	mov     $4*vect_system,%bx
	movw    $int_system,%es:(%bx)
	mov     %cs,%es:2(%bx)
	*/

	// Global vector

	mov     $4*vect_global,%bx
	movw    $0,%es:(%bx)
	mov     %ds,%es:2(%bx)

	pop     %es
	ret

//------------------------------------------------------------------------------

// Timer interrupt

	.global int_timer

int_timer:

	call    int_hand
	.word   0x08

// System call (synchronous)

/*
int_system:

	call    int_hand
	.word   0x80
*/

//------------------------------------------------------------------------------

int_hand:

	sti

	// Save registers to stack

	push    %es
	push    %ds

#ifdef CPU_186
	pusha
#else
	push    %ax
	push    %cx
	push    %dx
	push    %bx
	sub     $2,%sp  // skip dummy SP
	push    %bp
	push    %si
	push    %di
#endif

	// Switch to kernel data segment

	xor     %dx,%dx
	mov     %dx,%ds
	mov     kern_ds,%dx
	mov     %dx,%ds

	// Save old stack top

	mov     %sp,%bp
	mov     %ss,%cx

	// Increment & check task level
	// =1: was in user mode
	// >1: was in kernel mode

	cli
	mov     task_cur,%bx
	mov     task_level(%bx),%ax
	inc     %ax
	mov     %ax,task_level(%bx)
	cmp     $1,%ax
	jne     1f

	// Was in user mode
	// Switch to kernel stack

	mov     task_stack(%bx),%sp
	add     task_ssize(%bx),%sp
	mov     %dx,%ss

	// Push old top on new stack

1:	sti
	push    %cx
	push    %bp

	// Get interrupt number

	mov     %cx,%es
	mov     %es:regs_num(%bp),%si
	mov     %cs:(%si),%ax
	mov     %dx,%es

	// Call interrupt procedure
	// Old stack top already pushed

	push    %ax
	call    int_proc
	add     $2,%sp

int_exit:

	// Restore old stack top

	pop     %bp
	pop     %cx

	// Decrement task level
	// Switch back to old stack

	cli
	mov     task_cur,%bx
	decw    task_level(%bx)
	mov     %bp,%sp
	mov     %cx,%ss
	sti

#ifdef CPU_186
	popa
#else
	pop     %di
	pop     %si
	pop     %bp
	add     $2,%sp  // skip dummy SP
	pop     %bx
	pop     %dx
	pop     %cx
	pop     %ax
#endif

	pop    %ds
	pop    %es

	add    $2,%sp  // skip int_num *

	iret

//------------------------------------------------------------------------------

	.global	task_switch

// Task switch in kernel mode only
// within the same CS DS SS

// Synchronous call so AX BX CX DX not saved
// as scratch registers for GCC-IA16

// arg1: task prev *
// arg2: task next *

task_switch:

	push    %bp
	mov     %sp,%bp

	push    %es
	push    %si
	push    %di

	cli
	mov     4(%bp),%bx  // arg1
	or      %bx,%bx
	jz      1f
	mov     %sp,task_sp(%bx)

1:	mov     6(%bp),%bx  // arg2
	mov     task_sp(%bx),%sp
	sti

	pop     %di
	pop     %si
	pop     %es
	pop     %bp
	ret

//------------------------------------------------------------------------------

	.global task_init_kern

// arg1: task *
// arg2: kernel IP

task_init_kern:

	push    %bp
	mov     %sp,%bp

	mov     4(%bp),%bx  // arg1
	movw    $1,task_level(%bx)

	// Setup kernel stack
	// to exit from task_switch()
	// and branch to provided IP

	push    %di
	mov     task_stack(%bx),%di
	add     task_ssize(%bx),%di
	sub     $SWITCH_SIZE,%di
	mov     %di,task_sp(%bx)

	push    %es
	mov     %ss,%ax
	mov     %ax,%es

	cld
	xor     %ax,%ax
	stosw   // DI
	stosw   // SI
	mov     %ds,%ax
	stosw   // ES
	xor     %ax,%ax
	stosw   // BP
	mov     6(%bp),%ax  // arg2
	stosw   // kernel IP

	pop     %es
	pop     %di
	pop     %bp
	ret

//------------------------------------------------------------------------------

	.global task_init_user

//  User in small memory model SS=DS

//  +4: arg1: task *
//  +6: arg2: user IP
//  +8: arg3: user CS
// +10: arg4: user SP
// +12: arg5: user SS

task_init_user:

	push    %bp
	mov     %sp,%bp

	mov     4(%bp),%bx  // arg1

	// Setup kernel stack
	// to exit from task_switch()
	// and branch to int_exit

	mov     $int_exit,%ax
	push    %ax
	push    %bx
	call    task_init_kern
	add     $2*2,%sp

	// Setup user stack
	// to exit from int_hand

	push    %si
	push    %di
	mov     10(%bp),%di  // arg4
	mov     12(%bp),%dx  // arg5
	sub     $REGS_SIZE,%di
	mov     %di,%si

	push    %es
	mov     %dx,%es

	// cld by task_init_term()
	xor     %ax,%ax
	mov     $8,%cx  // 8 general registers
	rep
	stosw   // DI SI BP dummy-SP BX DX CX AX
	mov     %dx,%ax
	stosw   // user DS
	stosw   // user ES
	xor     %ax,%ax
	stosw   // int num *
	mov     6(%bp),%ax  // arg2
	stosw   // user IP
	mov     8(%bp),%ax  // arg3
	stosw   // user CS
	pushf
	pop     %ax
	stosw   // user FL

	// Push user top on kernel stack

	mov     %ss,%ax
	mov     %ax,%es
	mov     task_sp(%bx),%di
	sub     $2*2,%di
	mov     %di,task_sp(%bx)

	mov     %si,%ax
	stosw   // user SP
	mov     %dx,%ax
	stosw   // user SS

	pop     %di
	pop     %si
	pop     %es
	pop     %bp
	ret

//------------------------------------------------------------------------------
