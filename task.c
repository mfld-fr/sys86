
#include "arch.h"
#include "queue.h"
#include "task.h"
#include "int.h"
#include "wait.h"

// Array of tasks
// Priority is array order

struct task_s * tasks [TASK_MAX];

struct task_s * task_prev;
struct task_s * task_cur;
struct task_s * task_next;

// TODO: atomics ?
int sched_need;
int sched_lock;
int int_level;

struct task_s task_idle;
word_t stack_idle [STACK_SIZE];

// Task scheduler
// Priority is task array order

void schedule ()
	{
	word_t flags = int_save ();

	while (1) {
		// No scheduling if locked
		// Used by interrupt procedure
		// to forbid scheduling in nested interrupt

		if (sched_lock) break;
		sched_need = 0;

		struct task_s * n = NULL;

		for (int i = 0; i < TASK_MAX; i++) {
			struct task_s * t = tasks [i];
			if (t && t->stat == TASK_RUN) {
				n = t;
				break;
				}
			}

		// Default to idle task
		// to avoid continuing in waiting task

		if (!n) n = &task_idle;

		if (n != task_cur) {
			task_prev = task_cur;
			task_next = n;

			// No task switch until interrupt stack is empty
			if (!int_level) task_switch ();
			}

		break;
		}

	int_back (flags);
	}

// Wait for event occurrence

void event_wait (int event, cond_f test, void * param)
	{
	word_t flag;

	// TODO: no need to loop if unique waiter for event

	while (1) {
		// Atomic condition test & prepare to sleep

		flag = int_save ();
		if (test (param)) break;
		task_cur->wait = event;
		task_cur->stat = TASK_WAIT;
		int_back (flag);

		// Can sleep now

		schedule ();
		}

	int_back (flag);
	}

// Wake up task on event occurrence
// TODO: no need to reschedule
// if waking up lower prioritized than current

void task_event (int event)
	{
	word_t flags = int_save ();

	for (int i = 0; i < TASK_MAX; i++) {
		struct task_s * t = tasks [i];
		if (t && (t->stat == TASK_WAIT) && (t->wait == event)) {
			t->wait = EVENT_NONE;
			t->stat = TASK_RUN;
			sched_need++;
			}
		}

	if (sched_need) schedule ();

	int_back (flags);
	}

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
		//serial_read (&c);

		// TEST: wait for console input
		// Atomic condition test & go to sleep

		word_t flag;
		while (1) {
			flag = int_save ();
			if (con_peek (&c)) break;
			task_cur->wait = EVENT_CONSOLE_IN;
			task_cur->stat = TASK_WAIT;
			int_back (flag);
			schedule ();
			}

		int_back (flag);

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

		con_send (c);
		}
	}

void idle ()
	{
	while (1) halt ();
	}

// TODO: allocate stack on heap

void task_init_near (int i, struct task_s * t, void * entry, word_t * stack, word_t size)
	{
	t->level = 1;
	t->stack = stack;
	t->ssize = size;
	stack_init_near (t, entry, stack + size);
	t->stat = TASK_RUN;
	tasks [i] = t;
	}

void main ()
	{
	vect_init ();

	//queue_init (&queue_0);

	task_init_near (TASK_MAX - 1, &task_idle, idle, stack_idle, STACK_SIZE);

	task_init_near (0, &task_recv, main_recv, stack_recv, STACK_SIZE);
	task_init_near (1, &task_send, main_send, stack_send, STACK_SIZE);

	task_next = &task_recv;
	task_switch ();
	}
