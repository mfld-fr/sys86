#include "types.h"
#include "task.h"
#include "int.h"
#include "wait.h"
#include "io.h"
#include "queue.h"
#include "arch.h"

struct queue_s serial_in;
//struct queue_s serial_out;

void timer_proc (void)
	{
	// TODO: rewrite in C
	watchdog ();
	}

void serial_proc (void)
	{
	word_t stat = inw (IO_SERIAL_STATUS);

	if (stat & SERIAL_STATUS_RDR) {
		word_t c = inw (IO_SERIAL_RDATA);
		if (!queue_put (&serial_in, (byte_t) c))
			task_event (EVENT_SERIAL_IN);
		}
	}

int serial_read (byte_t * c)
	{
	event_wait (EVENT_SERIAL_IN, (cond_f) queue_not_empty, &serial_in);
	queue_get (&serial_in, c);
	return 1;
	}

void serial_send (byte_t c)
	{
	while (!(inw (IO_SERIAL_STATUS) & SERIAL_STATUS_TDR));
	outw (IO_SERIAL_TDATA, (word_t) c);
	}

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
		case 0x08:  // timer 0
		timer_proc ();
		break;

		case 0x14:  // serial
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
