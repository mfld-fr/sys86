
#include "timer.h"

	.macro time_sample index

	push   %ax
	push   %bx
	push   %dx

	mov     $IO_TIMER2_COUNT,%dx
	in      %dx,%ax

	lea    time_samples,%bx
	mov    %ax,2*\index(%bx)

	pop    %dx
	pop    %bx
	pop    %ax

	.endm
