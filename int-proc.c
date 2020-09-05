#include "types.h"
#include "task.h"
#include "int.h"
#include "wait.h"
#include "io.h"
#include "queue.h"

// struct queue_s serial_in;
// struct queue_s serial_out;

void timer_proc (void)
	{
	// TEST: horrible hack to wake up receive task
	// and to retry read from console

	task_event (EVENT_CONSOLE_IN);

	// TODO: rewrite in C
	// watchdog ();
	}

/*
void serial_proc (void)
	{
	word_t stat = in (IO_SERIAL_STATUS);

	if (stat & SERIAL_STATUS_RDR) {
		byte_t c = in (IO_SERIAL_RDATA);
		if (!queue_put (&serial_in, c))
			task_event (EVENT_CONSOLE_IN);
		}
	}
*/

/*
int serial_read (byte_t * c)
	{
	event_wait (EVENT_CONSOLE_IN, (cond_f) queue_not_empty, &serial_in);
	queue_get (&serial_in, c);
	return 1;
	}
*/

void int_end (word_t num)
	{
	//out (IO_INT_END, num);
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
		case 0x08:  // timer 0
		timer_proc ();
		break;

		/*
		case 0x14:  // serial
		serial_proc ();
		break;
		*/
		}

	int_end (num);

	flags = int_save ();
	sched_lock--;
	if (sched_need) schedule ();
	int_level--;
	int_back (flags);
	}
