
#include "arch.h"
#include "queue.h"
#include "task.h"
#include "int.h"

// Array of tasks
// Priority is array order

struct task_s * tasks [TASK_MAX];
struct task_s * task_cur;

static struct task_s task_idle;
static word_t stack_idle [STACK_SIZE];

// Task scheduler
// Priority is task array order

void task_sched () {

	while (1) {
		struct task_s * n = NULL;

		// TODO: disable interrupt

		for (int i = 0; i < TASK_MAX; i++) {
			struct task_s * t = tasks [i];
			if (t && t->stat == TASK_RUN) {
				n = t;
				break;
				}
			}

		// Default to idle task

		if (!n) n = &task_idle;

		if (n && n != task_cur) {
			struct task_s * p = task_cur;
			task_cur = n;
			task_switch (p, n);
			}

		break;
		}
	}

// Test code


static struct queue queue_0;

static struct task_s task_recv;
static struct task_s task_send;

static word_t stack_recv [STACK_SIZE];
static word_t stack_send [STACK_SIZE];

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

// TODO: allocate stack on heap

void task_init (int i, struct task_s * t, void * entry, word_t * stack, word_t size) {
	t->stack = stack;
	t->ssize = size;
	task_init_kern (t, entry);
	t->stat = TASK_RUN;
	tasks [i] = t;
}

void main () {
	vect_init ();

	//queue_init (&queue_0);

	task_init (TASK_MAX - 1, &task_idle, idle, stack_idle, STACK_SIZE);

	task_init (0, &task_recv, main_recv, stack_recv, STACK_SIZE);
	task_init (1, &task_send, main_send, stack_send, STACK_SIZE);

	task_cur = &task_recv;
	task_switch (NULL, &task_recv);
	}
