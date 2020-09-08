// SYS86 project
// Generic interrupt handling

#include "types.h"
#include "int.h"
#include "task.h"
#include "timer.h"
#include "serial.h"

// Asynchronous interrupt level
// Used to forbid task switch in nested interrupt

int int_level;

// Interrupt procedure

void int_proc (word_t vect)
	{
	word_t flags;

	flags = int_save ();
	// No task switch until interrupt stack is empty
	int_level++;
	// No scheduling in nested interrupts
	sched_lock++;
	int_back (flags);

	switch (vect) {
		case VECT_TIMER0:
		timer0_proc ();
		break;

		case VECT_SERIAL:
		serial_proc ();
		break;
		}

	int_end (vect);

	flags = int_save ();
	sched_lock--;
	if (sched_need) schedule ();
	int_level--;
	int_back (flags);
	}
