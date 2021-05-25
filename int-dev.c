// SYS86 project
// R8810 interrupt controller

#include "arch.h"
#include "int.h"

// Interrupt controller I/O

#define IO_INT_EOI    0xFF22
#define IO_INT_MASK   0xFF28
#define IO_INT_TIMER  0xFF32
#define IO_INT_SERIAL 0xFF44

void int_end (word_t vect)
	{
	// Specific EOI with interrupt vector

	outw (IO_INT_EOI, vect);
	}

void int_dev_init (void)
	{
	outw (IO_INT_TIMER,  0x0000); // unmasked - priority 0
	outw (IO_INT_SERIAL, 0x0001); // unmasked - priority 1
	}
