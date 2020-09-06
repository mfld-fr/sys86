//------------------------------------------------------------------------------
// SYS86 - Architecture specific routines for the Advantech SMNP-1000-B1 SBC
//------------------------------------------------------------------------------

// Register offsets

#define io_timer1_mode  0xFF5E
#define io_port0_data   0xFF74

	.code16

	.text

//------------------------------------------------------------------------------

// Quiet the hardware watchdog

	.global watchdog

watchdog:

	push   %ax
	push   %dx
	mov    $io_port0_data,%dx
	in     %dx,%ax
	xor    $1,%ax
	out    %ax,%dx
	pop    %dx
	pop    %ax
	iret

//------------------------------------------------------------------------------

// Architecture setup

	.global arch_setup

arch_setup:

	push   %ax
	push   %bx
	push   %dx

	// Disable timer @ 1000 Hz

	mov     $io_timer1_mode,%dx
	in      %dx,%ax
	and     $0x7FFF,%ax  // disable timer bit
	or      $0x4000,%ax  // unlock enable bit
	out     %ax,%dx

	pop    %dx
	pop    %bx
	pop    %ax
	ret

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// Halt processor and wait for interrupt

	.global halt

halt:

	hlt
	ret

//------------------------------------------------------------------------------

// I/O helpers

	.global inw

inw:

	mov     %sp,%bx
	mov     2(%bx),%dx
	in      %dx,%ax
	ret

	.global outw

outw:

	mov     %sp,%bx
	mov     2(%bx),%dx
	mov     4(%bx),%ax
	out     %ax,%dx
	ret

//------------------------------------------------------------------------------
