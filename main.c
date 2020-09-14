// SYS86 project
// Main program

#include "arch.h"
#include "int.h"
#include "task.h"
#include "timer.h"
#include "queue.h"
#include "serial.h"
#include "lib.h"

// Test code

static struct task_s task_echo;
static word_t stack_echo [STACK_SIZE];

static void send_diff (word_t begin, word_t end)
	{
	word_t diff = time_diff (begin, end);

	char_t str [4];
	byte_t len;
	word_to_hex (diff, str, &len);

	for (int s = 0; s < len; s++)
		serial_send (str [s]);

	}

static void main_echo (void)
	{
	while (1) {
		// Read from serial port
		// Blocking operation

		byte_t c;
		serial_read (&c);

		// Echo to serial port

		serial_send (c);

		// Send time samples

		send_diff (0, 1);
		serial_send (' ');
		send_diff (1, 2);
		serial_send (13);
		serial_send (10);
		}
	}

void main ()
	{
	// Interrupt disabled on entry
	// System initialization
	// TODO: move to system.c

	vect_init ();    // interrupt vectors
	//int_init ();   // interrupt controller
	timer_init ();   // timer device
	serial_init ();  // serial port
	task_init ();    // task manager

	//queue_init (&queue_0);

	task_init_near (0, &task_echo, main_echo, stack_echo, STACK_SIZE);

	// Switch to first task
	// Initial stack not needed any more
	// Interrupt enabled by default in task

	task_next = &task_echo;
	task_switch ();
	}
