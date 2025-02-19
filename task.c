// SYS86 project
// Task management

#include "types.h"
#include "arch.h"
#include "int.h"
#include "task.h"
#include "heap.h"


// Array of tasks

struct task_s * tasks [TASK_MAX];

struct task_s * task_prev;
struct task_s * task_now;
struct task_s * task_next;

static int sched_lock;
static int sched_need;

//------------------------------------------------------------------------------

// Idle task

static struct task_s task_idle;

static void idle (void)
	{
	while (1) halt ();
	}

//------------------------------------------------------------------------------

// Task scheduler

void task_lock ()
	{
	word_t flags;
	int_save (flags);
	sched_lock++;
	int_restore (flags);
	}

void task_unlock ()
	{
	word_t flags;
	int_save (flags);

	// May have to schedule after unlocking

	if (!(--sched_lock) && sched_need) task_sched ();
	int_restore (flags);
	}


void task_sched (void)
	{
	word_t flags;
	int_save (flags);

	while (1)
		{
		// Defer scheduling if locked

		if (sched_lock)
			{
			sched_need++;
			break;
			}

		sched_need = 0;

		struct task_s * n = NULL;

		// Priority as task array index
		// with 0 as highest priority

		for (int i = 0; i < TASK_MAX; i++)
			{
			struct task_s * t = tasks [i];
			if (t && t->stat == TASK_RUN)
				{
				n = t;
				break;
				}
			}

		// Default to idle task
		// to avoid continuing in waiting task

		if (!n) n = &task_idle;

		if (n != task_now)
			{
			task_prev = task_now;
			task_next = n;

			// No task switch during interrupt

			if (!int_level) task_switch ();
			}

		break;
		}

	int_restore (flags);
	}

//------------------------------------------------------------------------------

// Wait for event occurrence and optional condition

void task_wait (struct wait_s * wait, cond_f test, void * param, int single)
	{
	word_t flags;

	while (1)
		{
		// Atomic (condition test + prepare to sleep)

		int_save (flags);
		if (test && test (param))
			{
			int_restore (flags);
			break;
			}

		wait->t = task_now;
		task_now->stat = TASK_WAIT;
		int_restore (flags);

		// Interrupt enabled for a short time
		// to give a latest chance before sleeping

		task_sched ();

		// No need to test the condition again
		// if only this task is waiting for the object
		// and no other task would change the condition

		if (single) break;
		}
	}


// May wake up one task on event occurrence

void task_event (struct wait_s * wait)
	{
	word_t flags;
	int_save (flags);

	if (wait->t)
		{
		struct task_s * t = wait->t;
		wait->t = NULL;

		t->stat = TASK_RUN;

		task_sched ();
		}

	int_restore (flags);
	}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void task_init (void)
	{
	task_init_near (-1, &task_idle, idle, STACK_SIZE);
	}

//------------------------------------------------------------------------------
