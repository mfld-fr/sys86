
#include "arch.h"
#include "queue.h"
#include "task.h"
#include "int.h"

// Array of tasks
// Priority is array order

struct task * tasks [TASK_MAX];
struct task * task_cur;

static struct task task_idle;

// Task scheduler
// Priority is task array order

void task_sched () {

	while (1) {
		struct task * n = NULL;

		// TODO: disable interrupt

		for (int i = 0; i < TASK_MAX; i++) {
			struct task * t = tasks [i];
			if (t && t->stat == TASK_RUN) {
				n = t;
				break;
				}
			}

		// Default to idle task

		if (!n) n = &task_idle;

		if (n && n != task_cur) {
			struct task * p = task_cur;
			task_cur = n;
			task_switch (p, n);
			}

		break;
		}
	}

// Test code


static struct queue queue_0;

static struct task task_recv;
static struct task task_send;

void main_recv () {
	while (1) {
		byte_t c;

		while (1) {
			task_cur->stat = TASK_WAIT;
			if (recv_char (&c)) break;
			task_sched ();
			}
		task_cur->stat = TASK_RUN;

		// As only one queue writer
		// no need to loop on wait

		while (1) {
			task_cur->stat = TASK_WAIT;
			if (!queue_full (&queue_0)) break;
			task_sched ();
			}
		task_cur->stat = TASK_RUN;

		queue_put (&queue_0, c);

		task_send.stat = TASK_RUN;
		task_sched ();
		}
	}

void main_send () {
	while (1) {
		// As only one queue reader
		// no need to loop on wait

		while (1) {
			task_cur->stat = TASK_WAIT;
			if (!queue_empty (&queue_0)) break;
			task_sched ();
			}
		task_cur->stat = TASK_RUN;

		byte_t c;
		queue_get (&queue_0, &c);

		task_recv.stat = TASK_RUN;
		task_sched ();

		send_char (c);
		}
	}

static void idle () {
	while (1) {
		halt ();
		}
	}

void task_init () {
	task_init_kern (&task_idle, idle);
	task_recv.stat = TASK_RUN;

	tasks [TASK_MAX - 1] = &task_idle;
	}

void main () {
	vect_init ();

	//queue_init (&queue_0);

	task_init ();

	task_init_kern (&task_recv, main_recv);
	task_recv.stat = TASK_RUN;

	task_init_kern (&task_send, main_send);
	task_send.stat = TASK_RUN;

	tasks [0] = &task_recv;
	tasks [1] = &task_send;

	task_cur = &task_recv;
	task_switch (NULL, &task_recv);
	}
