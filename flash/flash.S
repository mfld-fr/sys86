//------------------------------------------------------------------------------
// FLASH - MON86 procedure to burn flash
//------------------------------------------------------------------------------

// Segments

#define seg_stage 0x1000
#define seg_prog0 0x9000
#define seg_prog1 0xF800

//------------------------------------------------------------------------------

	.code16

	.text

entry:

	push    %cx
	push    %dx
	push    %di
	push    %ds
	push    %es

	// erase flash block

	mov     $seg_prog1,%dx
	xor     %cx,%cx
	mov     $1,%ah
	int     $0x61
	or      %al,%al
	jnz     flash_fail

	// check block erased

	mov     $seg_prog1,%ax
	mov     %ax,%es
	xor     %di,%di

	mov     $0x800,%cx
	mov     $0xFFFF,%ax
	cld
	repe
	scasw
	jnz     flash_fail

	// write block

	mov     $seg_stage,%ax
	mov     %ax,%ds
	xor     %si,%si

	mov     $seg_prog1,%dx
	xor     %cx,%cx
	mov     $2,%ah
	int     $0x61
	or      %al,%al
	jnz     flash_fail

	// check block write

	xor     %si,%si
	xor     %di,%di
	mov     $0x800,%cx
	repe
	cmpsw
	jnz     flash_fail

	xor     %ax,%ax
	jmp     flash_exit

flash_fail:

	mov     $1,%ax

flash_exit:

	pop     %es
	pop     %ds
	pop     %di
	pop     %dx
	pop     %cx
	retf

//------------------------------------------------------------------------------

