// SYS86 project
// R8810 interrupt controller

#include "arch.h"
#include "int.h"

// Interrupt controller I/O

#define IO_INT_EOI    0xFF22
#define IO_INT_MASK   0xFF28
#define IO_INT_TIMER  0xFF32
#define IO_INT_SERIAL 0xFF44


// Specific EOI with interrupt vector

void int_end (byte_t vect)
	{
	outw (IO_INT_EOI, (word_t) vect);
	}


// Unmask and set priority

void int_enable (byte_t vect)
	{
	switch (vect)
		{
		case VECT_TIMER0:
			outw (IO_INT_TIMER,  0x0000); // unmasked - priority 0
			break;

		case VECT_SERIAL:
			outw (IO_INT_SERIAL, 0x0001); // unmasked - priority 1
			break;

		}
	}
