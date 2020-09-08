// SYS86

// TODO: merge task.h / wait.h

#include "arch.h"
#include "int.h"
#include "task.h"
#include "wait.h"
#include "queue.h"
#include "serial.h"

// Test code

struct queue_s queue_0;

struct task_s task_recv;
struct task_s task_send;

word_t stack_recv [STACK_SIZE];
word_t stack_send [STACK_SIZE];

void main_recv ()
	{
	while (1) {
		// Read from console
		// Blocking operation

		byte_t c;
		serial_read (&c);

		// Wait for space in queue

		event_wait (EVENT_QUEUE_NOT_FULL, (cond_f) queue_not_full, &queue_0);

		queue_put (&queue_0, c);
		task_event (EVENT_QUEUE_NOT_EMPTY);  // not more empty
		}
	}

void main_send ()
	{
	while (1) {
		// Wait for data in queue

		event_wait (EVENT_QUEUE_NOT_EMPTY, (cond_f) queue_not_empty, &queue_0);

		byte_t c;
		queue_get (&queue_0, &c);
		task_event (EVENT_QUEUE_NOT_FULL);  // not more full

		serial_send (c);
		}
	}

void main ()
	{
	// Interrupt disabled on entry

	vect_init ();
	serial_init ();
	task_init ();

	//queue_init (&queue_0);

	task_init_near (0, &task_recv, main_recv, stack_recv, STACK_SIZE);
	task_init_near (1, &task_send, main_send, stack_send, STACK_SIZE);

	task_next = &task_recv;
	task_switch ();
	}
