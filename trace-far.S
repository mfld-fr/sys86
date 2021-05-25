
#define _ASSEMBLY

#include "int.h"
#include "timer.h"

	.code16

	.text

#ifdef CONFIG_TRACE

	.global trace_far

trace_far:

	push    %bp
	mov     %sp,%bp
	push    %ax
	push    %bx
	push    %dx
	push    %ds

	mov     $KERNEL_DS,%ax
	mov     %ax,%ds

	testw   $1,trace_on
	jz      1f

	mov     trace_count,%bx
	cmp     $TRACE_MAX,%bx
	jae     1f

	shl     $3,%bx
	add     $traces,%bx

	mov     4(%bp),%ax
	mov     %ax,(%bx)

	mov     $IO_TIMER2_COUNT,%dx
	in      %dx,%ax
	mov     %ax,2(%bx)

	mov     $IO_TIMER0_COUNT,%dx
	in      %dx,%ax
	mov     %ax,4(%bx)

	incw    trace_count

1:	pop     %ds
	pop     %dx
	pop     %bx
	pop     %ax
	pop     %bp
	ret

#endif // CONFIG_TRACE
