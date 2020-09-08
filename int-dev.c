// SYS86 project
// R8810 interrupt controller

#include "arch.h"
#include "int.h"

// Interrupt controller I/O

#define IO_INT_EOI 0xFF22

void int_end (word_t vect)
	{
	// Specific EOI with interrupt vector

	outw (IO_INT_EOI, vect);
	}
