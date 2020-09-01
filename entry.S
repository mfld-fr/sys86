
	.code16

	.text

	.global entry

entry:

	// Setup data & stack segment
	// in tiny memory model

	mov    %cs,%ax
	mov    %ax,%ds
	mov    %ax,%es

	xor    %dx,%dx
	mov    %ax,%ss  // auto CLI
	mov    %dx,%sp

	// Reset BSS section

	mov    $bss_begin,%di
	xor    %cx,%cx
	sub    %di,%cx
	xor    %al,%al
	cld
	rep
	stosb

	jmp    main

	.bss

	.global bss_begin

bss_begin:
	.word 0
