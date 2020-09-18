// SYS86 project
// Main entry point

	.code16

	.text

	.global entry

//------------------------------------------------------------------------------

entry:

	// Setup data segment
	// in tiny memory model

	mov    %cs,%ax
	mov    %ax,%ds
	mov    %ax,%es

	// Temporary stack at end of data segment
	// until first task switch and heap initialization

	cli
	mov    %ax,%ss
	xor    %ax,%ax
	mov    %ax,%sp
	sti

	// Reset BSS section
	// as required by GCC-IA16

	mov    $bss_begin,%di
	xor    %cx,%cx
	sub    %di,%cx
	xor    %al,%al
	cld

	// REP STOSB too long for HW watchdog
	// so split instruction to allow interrupt
	// and MON86 timer tick to quiet the dog

1:	stosb
	loop    1b

	// Main C function

	call    main

	// Unexpected return

1:	hlt
	jmp     1b

//------------------------------------------------------------------------------

	.bss

	// TODO: replace by linker variable

	.global bss_begin

bss_begin:
	.word 0

//------------------------------------------------------------------------------
