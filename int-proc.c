#include "types.h"
#include "int.h"
#include "task.h"
#include "io.h"
#include "timer.h"
#include "serial.h"

int int_level;

// TODO: move to int-dev.c
void int_end (word_t num)
	{
	outw (IO_INT_EOI, num);
	}

void int_proc (word_t num)
	{
	word_t flags;

	flags = int_save ();
	// No task switch until interrupt stack is empty
	int_level++;
	// No scheduling in nested interrupts
	sched_lock++;
	int_back (flags);

	switch (num) {
		case 0x08:  // timer 0 VECT_TIMER
		timer_proc ();
		break;

		case 0x14:  // serial VECT_SERIAL
		serial_proc ();
		break;
		}

	int_end (num);

	flags = int_save ();
	sched_lock--;
	if (sched_need) schedule ();
	int_level--;
	int_back (flags);
	}
