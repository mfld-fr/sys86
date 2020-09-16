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

static struct task_s task_recv;
static struct task_s task_send;
static word_t stack_recv [STACK_SIZE];
static word_t stack_send [STACK_SIZE];

static struct queue_s queue_0;
static struct wait_s wait_0;

static void send_diff (word_t begin, word_t end)
	{
	word_t diff = time_diff (begin, end);

	char_t str [4];
	byte_t len;
	word_to_hex (diff, str, &len);

	for (int s = 0; s < len; s++)
		serial_send (str [s]);

	}

static void main_recv (void)
	{
	while (1) {
		// Read from serial port
		// Blocking operation

		byte_t c = serial_read ();
		time_sample (3);

		// Send time samples

		send_diff (0, 1);
		serial_send (' ');
		send_diff (1, 2);
		serial_send (' ');
		send_diff (2, 3);
		serial_send (' ');

		// Put into intertask queue

		time_sample (1);
		task_wait (&wait_0, (cond_f) queue_not_full, &queue_0, 1);
		queue_put (&queue_0, c);
		task_event (&wait_0);  // not more empty
		}
	}

static void main_send (void)
	{
	while (1) {
		// Wait for data in queue

		task_wait (&wait_0, (cond_f) queue_not_empty, &queue_0, 1);
		byte_t c = queue_get (&queue_0);
		task_event (&wait_0);  // not more full
		time_sample (3);

		// Send to serial port

		//serial_send (c);

		// Send time samples

		send_diff (1, 2);
		serial_send (' ');
		send_diff (2, 3);
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

	task_init_near (0, &task_send, main_send, stack_send, STACK_SIZE);
	task_init_near (1, &task_recv, main_recv, stack_recv, STACK_SIZE);

	// Switch to first task
	// Initial stack not needed any more
	// Interrupt enabled by default in task

	task_next = &task_send;
	task_switch ();
	}
