//------------------------------------------------------------------------------
// SYS86 - Architecture specific routines for the Advantech SMNP-1000-B1 SBC
//------------------------------------------------------------------------------

// Register offsets

#define io_timer1_mode  0xFF5E
#define io_port0_data   0xFF74

// Interrupt vectors

#define vect_tick       0x1C

	.code16

	.text

//------------------------------------------------------------------------------

// Tick interrupt
// Called by the BIOS @ 1 Hz
// Used to quiet the hardware watchdog

int_tick:

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

	// Insert tick handler @ 1 Hz

	push   %ds
	xor    %ax,%ax
	mov    %ax,%ds

	mov    $vect_tick*4,%bx
	mov    $int_tick,%ax
	cli
	mov    %ax,(%bx)
	mov    %cs,2(%bx)
	sti

	pop    %ds

	pop    %dx
	pop    %bx
	pop    %ax
	ret

//------------------------------------------------------------------------------

// Read one character from serial
// Using Advantech tweaked BIOS

	.global recv_char

recv_char:

	push   %bp
	mov    %sp,%bp
	push   %bx
	mov    4(%bp),%bx  // arg1 = char * c

read_loop:

	mov    $0x10,%ah  // get extended key
	int    $0x16      // BIOS keyboard service
	or     %ah,%ah
	jz     read_exit  // got a key

	// Idle until next interrupt

	hlt
	jmp    read_loop

read_exit:

	mov    %al,(%bx)
	pop    %bx
	xor    %ax,%ax
	pop    %bp
	ret

// Write one character to serial
// Using Advantech tweaked BIOS

	.global send_char

send_char:

	push   %bp
	mov    %sp,%bp
	mov    4(%bp),%ax  // GCC-IA16 pushes char_t as word
	mov    $0x0A,%ah   // write character
	int    $0x10       // BIOS video service
	xor    %ax,%ax
	pop    %bp
	ret

// Write string to serial
// Using Advantech tweaked BIOS

	.global send_string

send_string:

	push   %bp
	mov    %sp,%bp
	push   %cx
	mov    6(%bp),%cx  // arg2 = word_t : string length
	push   %bp
	mov    4(%bp),%bp  // arg1 = char * : string pointer (ES already set)
	mov    $0x13,%ah   // write string
	int    $0x10       // BIOS video service
	pop    %bp
	pop    %cx
	xor    %ax,%ax
	pop    %bp
	ret

//------------------------------------------------------------------------------

