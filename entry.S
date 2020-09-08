// SYS86 project
// Main entry point

	.code16

	.text

	.global entry

//------------------------------------------------------------------------------

entry:
	// Disable interrupt until complete initialization

	cli

	// Setup data & stack segment
	// in tiny memory model

	mov    %cs,%ax
	mov    %ax,%ds
	mov    %ax,%es

	// Temporary stack at end of data segment
	// until first task switch and heap initialization

	mov    %ax,%ss
	xor    %ax,%ax
	mov    %ax,%sp

	// Reset BSS section
	// as required by GCC-IA16

	mov    $bss_begin,%di
	xor    %cx,%cx
	sub    %di,%cx
	xor    %al,%al
	cld
	rep
	stosb

	jmp    main

//------------------------------------------------------------------------------

	.bss

	.global bss_begin

bss_begin:
	.word 0

//------------------------------------------------------------------------------
