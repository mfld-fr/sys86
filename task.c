
#include "arch.h"
#include "queue.h"
#include "task.h"
#include "int.h"

#define TASK_MAX 2

static struct task * tasks [TASK_MAX];

// Task scheduler
// Priority is task array order

void task_sched () {

	struct task * n = NULL;

	for (int i = 0; i < TASK_MAX; i++) {
		struct task * t = tasks [i];
		if (t && t->stat == TASK_RUN) {
			n = t;
			break;
			}
		}

	if (n && n != task_cur) {
		struct task * p = task_cur;
		task_cur = n;
		task_switch (p, n);
		}
	}

// Test code

static struct queue queue_0;

static struct task task_recv;
static struct task task_send;

void main_recv () {
	while (1) {
		task_cur->stat = TASK_WAIT;
		if (queue_full (&queue_0)) task_sched ();
		task_cur->stat = TASK_RUN;

		byte_t c;
		recv_char (&c);
		queue_put (&queue_0, c);

		task_send.stat = TASK_RUN;
		task_sched ();
		}
	}

void main_send () {
	while (1) {
		task_cur->stat = TASK_WAIT;
		if (queue_empty (&queue_0)) task_sched ();
		task_cur->stat = TASK_RUN;

		byte_t c;
		queue_get (&queue_0, &c);
		send_char (c);

		task_recv.stat = TASK_RUN;
		task_sched ();
		}
	}

void main () {
	vect_init ();

	queue_init (&queue_0);

	task_init_kern (&task_recv, main_recv);
	task_recv.stat = TASK_RUN;

	task_init_kern (&task_send, main_send);
	task_send.stat = TASK_RUN;

	// Emptying the queue first

	tasks [0] = &task_send;
	tasks [1] = &task_recv;

	task_cur = &task_send;
	task_switch (NULL, &task_send);
	}
