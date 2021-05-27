// SYS86 project
// Generic interrupt handling

#include "types.h"
#include "int.h"
#include "task.h"
#include "timer.h"
#include "serial.h"
#include "heap.h"


// Asynchronous interrupt level
// Used to forbid task switch in nested interrupt

int int_level;

byte_t * int_stack_base;
byte_t * int_stack_top;

// Interrupt procedure

void int_proc (byte_t vect)
	{
	word_t flags;

	int_save (flags);
	// No scheduling in interrupts
	sched_lock++;
	int_restore (flags);

	switch (vect) {
		case VECT_TIMER0:
		timer0_proc ();
		break;

		case VECT_SERIAL:
		serial_proc ();
		break;
		}

	int_end (vect);

	int_save (flags);
	sched_lock--;
	if (sched_need) task_sched ();
	int_restore (flags);
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
