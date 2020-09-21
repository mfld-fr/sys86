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

#ifdef CONFIG_TRACE
	pushw   $0x10
	call    trace_far
	add     $2,%sp
#endif // CONFIG_TRACE

	call    int_hand
	.word   VECT_TIMER0

// Serial interrupt

	.global int_serial

int_serial:

#ifdef CONFIG_TRACE
	pushw   $0x20
	call    trace_far
	add     $2,%sp
#endif // CONFIG_TRACE

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

#ifdef CONFIG_INT_USER
#include "int-user.S"
#endif // CONFIG_INT_USER

#ifdef CONFIG_INT_KERN
#include "int-kern.S"
#endif // CONFIG_INT_KERN


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
