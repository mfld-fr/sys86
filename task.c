// EMU86 project
// Task management

#include "types.h"
#include "arch.h"
#include "int.h"
#include "task.h"
#include "heap.h"


// Array of tasks

struct task_s * tasks [TASK_MAX];

struct task_s * task_prev;
struct task_s * task_cur;
struct task_s * task_next;

int sched_need;
int sched_lock;

//------------------------------------------------------------------------------

// Idle task

static struct task_s task_idle;

static void idle (void)
	{
	while (1) halt ();
	}

//------------------------------------------------------------------------------

// Task scheduler

void task_sched (void)
	{
	word_t flags = int_save ();

	while (1) {
		// No scheduling if locked
		// Used by interrupt procedure
		// to forbid scheduling during interrupt

		if (sched_lock) break;
		sched_need = 0;

		struct task_s * n = NULL;

		// Priority in task array order
		// with 0 as highest priority

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

void task_wait (struct wait_s * wait, cond_f test, void * param, int single)
	{
	word_t flag;

	while (1) {
		// Atomic condition test & prepare to sleep

		flag = int_save ();
		if (test (param)) break;
		task_cur->wait = wait;
		wait->t = task_cur;
		task_cur->stat = TASK_WAIT;
		int_back (flag);

		// Interrupt enabled for a short time
		// to give a latest chance before sleeping

		task_sched ();

		// No need to loop if single waiter on object

		if (single) return;
		}

	int_back (flag);
	}

// Wake up task on event occurrence
// TODO: no need to reschedule
// if waking up lower prioritized than current

void task_event (struct wait_s * wait)
	{
	word_t flags = int_save ();

	if (wait->t) {
		wait->t->wait = NULL;
		wait->t->stat = TASK_RUN;
		wait->t = NULL;
		sched_need++;
		}

	if (sched_need) task_sched ();

	int_back (flags);
	}


void task_init_near (int i, struct task_s * t, void * entry, word_t size)
	{
	// FIXME: check returned value
	t->stack = heap_alloc (size * sizeof (word_t), HEAP_TAG_STACK);
	t->ssize = size;

	stack_init_near (t, entry, t->stack + t->ssize);

	t->stat = TASK_RUN;
	if (i >= 0) tasks [i] = t;
	}


void task_init_far (int i, struct task_s * t, word_t seg, word_t size)
	{
	// FIXME: check returned value
	t->stack = heap_alloc (size * sizeof (word_t), HEAP_TAG_STACK);
	t->ssize = size;

	t->inter = 1;
	stack_init_far (t, 0, seg, 0, seg);

	t->stat = TASK_RUN;
	if (i >= 0) tasks [i] = t;
	}

void task_init (void)
	{
	task_init_near (-1, &task_idle, idle, STACK_SIZE);
	}
