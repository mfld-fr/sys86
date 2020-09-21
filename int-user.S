// SYS86 project
// Architecture specific interrupt handling
// Context on user stack

//------------------------------------------------------------------------------

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

	// Switch to kernel data segment

	xor     %dx,%dx
	mov     %dx,%ds
	mov     4*VECT_GLOBAL+2,%dx
	mov     %dx,%ds

	// Save old stack top

	mov     %sp,%bp
	mov     %ss,%cx

	// Increment & check task level
	// =1: was in user mode
	// >1: was in kernel mode

	mov     task_cur,%si
	mov     task_level(%si),%ax
	inc     %ax
	mov     %ax,task_level(%si)
	cmp     $1,%ax
	jne     1f

	// Was in user mode
	// Switch to kernel stack

	mov     %bp,USER_SP(%si)
	mov     %cx,USER_SS(%si)

	mov     task_stack(%si),%sp
	add     task_ssize(%si),%sp
	mov     %dx,%ss

	// Get interrupt number

1:	sti

	mov     %cx,%es
	mov     %es:regs_num(%bp),%bx
	mov     %cs:(%bx),%ax
	mov     %dx,%es

	// Call interrupt procedure
	// with far pointer to saved context

	push    %cx
	push    %bp
	push    %ax
	call    int_proc
	add     $2*3,%sp

	// Keep interrupt disabled
	// until all stack switches completed
	// and context fully restored

	cli

	// Decrement & check task level
	// =0: back to user mode
	// >0: still in kernel mode

	//mov     task_cur,%si
	mov     task_level(%si),%ax
	dec     %ax
	mov     %ax,task_level(%si)
	jnz     int_exit

	// Back to user mode
	// Switch to user stack

	mov     USER_SP(%si),%sp
	mov     USER_SS(%si),%ss

	// Now the interrupt stack is empty
	// and context on top of interrupted stack
	// so task switching can occur

int_exit:

	// May save previous stack top

	mov     task_prev,%bx
	or      %bx,%bx
	jz      1f

	mov     %sp,TOP_SP(%bx)
	mov     %ss,TOP_SS(%bx)

	xor     %bx,%bx
	mov     %bx,task_prev

	// May switch to next stack top

1:	mov     task_next,%bx
	or      %bx,%bx
	jz      1f

	mov     TOP_SP(%bx),%sp
	mov     TOP_SS(%bx),%ss

	mov     %bx,task_cur
	xor     %bx,%bx
	mov     %bx,task_next

	// Restore registers from stack

1:	pop     %di
	pop     %si
	pop     %bp
	pop     %bx
	pop     %dx
	pop     %cx
	pop     %ax

	pop     %ds
	pop     %es

	add     $2,%sp  // skip int_num *

	// Interrupt return for remaining IP CS FL

#ifdef CONFIG_TRACE
	pushw   $0x30
	call    trace_far
	add     $2,%sp
#endif // CONFIG_TRACE

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

	sub     $2,%sp  // skip int_num *

	push    %es
	push    %ds

	push    %ax
	push    %cx
	push    %dx
	push    %bx
	push    %bp
	push    %si
	push    %di

	// Rearrange IP CS FL

	mov     %sp,%bp
	mov     regs_ip(%bp),%ax
	xchg    %ax,regs_fl(%bp)
	mov     %ax,regs_ip(%bp)

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

	push    %ss
	push    8(%bp)  // arg3 = SP
	push    %cs
	push    6(%bp)  // arg2 = IP
	push    4(%bp)  // arg1 = task *

	call    stack_init_far
	add     $2*5,%sp

	pop     %bp
	ret

//------------------------------------------------------------------------------

