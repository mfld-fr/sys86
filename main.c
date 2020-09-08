// SYS86 project
// Main program

#include "arch.h"
#include "int.h"
#include "task.h"
#include "timer.h"
#include "queue.h"
#include "serial.h"

// Test code

static struct queue_s queue_0;

static struct task_s task_recv;
static struct task_s task_send;

static word_t stack_recv [STACK_SIZE];
static word_t stack_send [STACK_SIZE];

static void main_recv (void)
	{
	while (1) {
		// Read from console
		// Blocking operation

		byte_t c;
		serial_read (&c);

		// Wait for space in queue

		task_wait (&queue_0, (cond_f) queue_not_full);

		queue_put (&queue_0, c);
		task_event (&queue_0);  // not more empty
		}
	}

static void main_send (void)
	{
	while (1) {
		// Wait for data in queue

		task_wait (&queue_0, (cond_f) queue_not_empty);

		byte_t c;
		queue_get (&queue_0, &c);
		task_event (&queue_0);  // not more full

		serial_send (c);
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
	task_init ();    // tasks & scheduler

	//queue_init (&queue_0);

	task_init_near (0, &task_recv, main_recv, stack_recv, STACK_SIZE);
	task_init_near (1, &task_send, main_send, stack_send, STACK_SIZE);

	// Switch to first task
	// Initial stack not needed any more
	// Interrupt enabled by default in task

	task_next = &task_recv;
	task_switch ();
	}
