// EMU86 project
// Task management

#include "types.h"
#include "arch.h"
#include "int.h"
#include "task.h"

// Array of tasks
// Priority is array order

struct task_s * tasks [TASK_MAX];

struct task_s * task_prev;
struct task_s * task_cur;
struct task_s * task_next;

int sched_need;
int sched_lock;

//------------------------------------------------------------------------------

// Idle task

static struct task_s task_idle;
static word_t stack_idle [STACK_SIZE];

static void idle (void)
	{
	while (1) halt ();
	}

//------------------------------------------------------------------------------

// Task scheduler
// Priority is task array order

void schedule (void)
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

void task_wait (void * object, cond_f test)
	{
	word_t flag;

	// TODO: no need to loop if unique waiter for event

	while (1) {
		// Atomic condition test & prepare to sleep

		flag = int_save ();
		if (test (object)) break;
		task_cur->wait = object;
		task_cur->stat = TASK_WAIT;
		int_back (flag);

		// Interrupt enabled for a short time
		// to give a chance before sleeping

		schedule ();
		}

	int_back (flag);
	}

// Wake up task on event occurrence
// TODO: no need to reschedule
// if waking up lower prioritized than current

void task_event (void * object)
	{
	word_t flags = int_save ();

	for (int i = 0; i < TASK_MAX; i++) {
		struct task_s * t = tasks [i];
		if (t && (t->stat == TASK_WAIT) && (t->wait == object)) {
			t->wait = NULL;
			t->stat = TASK_RUN;
			sched_need++;
			}
		}

	if (sched_need) schedule ();

	int_back (flags);
	}

// TODO: allocate stack on heap to minimize BSS section

void task_init_near (int i, struct task_s * t, void * entry, word_t * stack, word_t size)
	{
	t->level = 1;
	t->stack = stack;
	t->ssize = size;
	stack_init_near (t, entry, stack + size);
	t->stat = TASK_RUN;
	tasks [i] = t;
	}

void task_init (void)
	{
	task_init_near (TASK_MAX - 1, &task_idle, idle, stack_idle, STACK_SIZE);
	}
