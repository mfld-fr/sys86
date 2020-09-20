
#include "timer.h"

	.macro time_sample index

	push    %ax
	push    %bx
	push    %dx
	push    %ds

	xor     %ax,%ax
	mov     %ax,%ds
	mov     $VECT_GLOBAL*4+2,%bx
	mov     (%bx),%ds

	mov     $IO_TIMER1_COUNT,%dx
	in      %dx,%ax

	mov     $time_samples,%bx
	mov     %ax,2*\index(%bx)

	pop     %ds
	pop     %dx
	pop     %bx
	pop     %ax

	.endm
