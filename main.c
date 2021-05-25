// SYS86 project
// Main program

#include "arch.h"
#include "int.h"
#include "task.h"
#include "timer.h"
#include "queue.h"
#include "serial.h"
#include "lib.h"
#include "heap.h"


// Test code

static struct task_s task_recv;
static struct task_s task_send;
static struct task_s task_stub;

static struct queue_s queue_0;
static struct wait_s wait_0;

//------------------------------------------------------------------------------

#ifdef CONFIG_TRACE

static struct task_s task_dump;

static void trace_dump (void)
	{
	char_t str [4];
	byte_t len;

	trace_on = 0;

	for (word_t i = 0; i < trace_count; i++)
		{
		word_t num = traces [i].num;
		len = word_to_hex (num, str);

		for (int s = 0; s < len; s++)
			serial_send (str [s]);

		serial_send (' ');

		word_t msec = traces [i].msec;
		len = word_to_hex (msec, str);

		for (int s = 0; s < len; s++)
			serial_send (str [s]);

		serial_send ('.');

		word_t usec = traces [i].usec / 5;
		len = word_to_hex (usec, str);

		for (int s = 0; s < len; s++)
			serial_send (str [s]);

		serial_send (13);
		serial_send (10);
		}

	serial_send (13);
	serial_send (10);

	trace_count = 0;
	}

static void main_dump (void)
	{
	while (1)
		{
		// Wait for timer

		timer_get ();

		// Calibrate trace overhead

		trace_near (0x300);
		trace_near (0x301);

		// Dump traces

		trace_dump ();
		trace_on = 1;
		}
	}

#endif // CONFIG_TRACE

//------------------------------------------------------------------------------

static void main_recv (void)
	{
	while (1)
		{
		// Read from serial port
		// Blocking operation

		byte_t c = serial_get ();
		//trace_near (0x100);

		// Put into intertask queue

		task_wait (&wait_0, (cond_f) queue_not_full, &queue_0, 1);
		queue_put (&queue_0, c);
		task_event (&wait_0);  // not more empty
		}
	}

static void main_send (void)
	{
	while (1)
		{
		// Wait for data in queue

		task_wait (&wait_0, (cond_f) queue_not_empty, &queue_0, 1);
		byte_t c = queue_get (&queue_0);
		task_event (&wait_0);  // not more full
		//trace_near (0x200);

		// Send to serial port

		serial_send (c);
		//trace_near (0x250);
		}
	}


extern int _free_begin;  // linker symbol

int main ()
	{
	// System initialization

	heap_init ();
	heap_add (&_free_begin, 0x8000);

	int_init ();
	vect_init ();    // interrupt vectors
	int_dev_init (); // interrupt controller
	timer_init ();   // timer device
	serial_init ();  // serial port
	task_init ();    // task manager

	task_init_near (0, &task_send, main_send, STACK_SIZE);
	task_init_near (1, &task_recv, main_recv, STACK_SIZE);

#ifdef CONFIG_TRACE
	task_init_near (2, &task_dump, main_dump, STACK_SIZE);
#endif // CONFIG_TRACE

	// Idle task in user space

	task_init_far (3, &task_stub, 0x2000, STACK_SIZE);

	// Switch to first task
	// Initial stack not needed any more
	// Interrupt enabled by default in task

#ifdef CONFIG_TRACE
	trace_on = 1;
#endif // CONFIG_TRACE

	task_next = &task_send;
	task_switch ();
	return 0;
	}
