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

static struct task_s task_loop;
static struct task_s task_stub;


//------------------------------------------------------------------------------

#ifdef CONFIG_TRACE

static void trace_dump (void)
	{
	char_t str [5];
	byte_t len;

	trace_on = 0;

	for (word_t i = 0; i < trace_count; i++)
		{
		word_t num = traces [i].num;
		len = word_to_hex (num, str);

		for (int s = 0; s < len; s++)
			serial_put (str [s]);

		serial_put (' ');

		word_t msec = traces [i].msec;
		len = word_to_dec (msec, str);

		for (int s = 0; s < len; s++)
			serial_put (str [s]);

		serial_put ('.');

		word_t usec = traces [i].usec / 5;
		len = word_to_dec (usec, str);

		for (int s = 0; s < len; s++)
			serial_put (str [s]);

		serial_put (13);
		serial_put (10);
		}

	serial_put (13);
	serial_put (10);

	trace_count = 0;
	trace_on = 1;
	}

#endif // CONFIG_TRACE

#ifdef HEAP_DEBUG

void heap_dump (heap_s * h)
	{
	char_t str [5];
	byte_t len;

	len = word_to_hex ((word_t) h, str);
	for (int s = 0; s < len; s++)
		serial_put (str [s]);

	serial_put (' ');

	len = word_to_hex (h->tag, str);
	for (int s = 0; s < len; s++)
		serial_put (str [s]);

	serial_put (' ');

	len = word_to_dec (h->size, str);
	for (int s = 0; s < len; s++)
		serial_put (str [s]);

	serial_put (13);
	serial_put (10);
	}

#endif // HEAP_DEBUG


static struct task_s task_dump;

static void main_dump (void)
	{
	while (1)
		{
		// Wait for timer

		timer_get ();

		// Dump traces

#ifdef CONFIG_TRACE
		trace_dump ();
#endif

		// Dump heap

#ifdef HEAP_DEBUG
		heap_iterate (heap_dump);

		serial_put (13);
		serial_put (10);
#endif

		}
	}


//------------------------------------------------------------------------------

static void main_loop (void)
	{
	while (1)
		{
		// Read from serial port
		// Blocking operation

		byte_t c = serial_get ();
		//trace_near (0x100);

		// Write to serial port
		// Blocking operation

		serial_put (c);
		//trace_near (0x200);
		}
	}

extern int _free_begin;  // linker symbol

int main ()
	{
	// System initialization

	heap_init ();
	heap_add (&_free_begin, 0x8000);

	int_init ();
	// FIXME: set vector after device initialization (vect_set)
	vect_init ();    // interrupt vectors
	int_dev_init (); // interrupt controller
	timer_init ();   // timer device
	task_init ();    // task manager
	serial_init ();  // serial port

	task_init_near (0, &task_loop, main_loop, STACK_SIZE);

	task_init_near (2, &task_dump, main_dump, STACK_SIZE);

	// Idle task in user space

	task_init_far (4, &task_stub, 0x2000, STACK_SIZE);

	// Switch to first task
	// Initial stack not needed any more
	// Interrupt enabled by default in task

#ifdef CONFIG_TRACE
	trace_on = 1;
#endif // CONFIG_TRACE

	task_next = &task_loop;
	task_switch ();
	return 0;
	}
