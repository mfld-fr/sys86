// SYS86 project
// Generic interrupt handling

#include "types.h"
#include "int.h"
#include "task.h"
#include "timer.h"
#include "serial.h"
#include "heap.h"


// Asynchronous interrupt nesting level

int int_level;

// Asynchronous interrupt stack
// Allow smaller task stacks

byte_t * int_stack_base;
byte_t * int_stack_top;


// Interrupt procedure

void int_proc (byte_t vect)
	{
	// No scheduling in interrupt
	// if any task event occurs

	task_lock ();

	switch (vect)
		{
		case VECT_TIMER0:
		timer0_proc ();
		break;

		case VECT_SERIAL:
		serial_proc ();
		break;
		}

	int_end (vect);

	task_unlock ();
	}


void int_init (void)
	{
	int_stack_base = heap_alloc (INT_STACK_SIZE, HEAP_TAG_STACK);
	int_stack_top = int_stack_base + INT_STACK_SIZE;

	vect_set (VECT_SYSTEM, int_system);
	}


void int_set_timer (void)
	{
	vect_set (VECT_TIMER0, int_timer0);
	int_enable (VECT_TIMER0);
	}

void int_set_serial (void)
	{
	vect_set (VECT_SERIAL, int_serial);
	int_enable (VECT_SERIAL);
	}
