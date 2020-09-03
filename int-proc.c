#include "types.h"
#include "task.h"
#include "int.h"
#include "wait.h"

void int_proc (word_t num)
	{
	word_t flags;

	// TODO: atomic increment
	flags = int_save ();
	sched_lock++;
	int_back (flags);

	if (num == 0x08) {
		// Horrible hack to wake up receive task
		// in order to retry to peek key

		task_event (EVENT_CONSOLE_IN);
		}

	flags = int_save ();
	sched_lock--;
	if (sched_need) schedule ();
	int_back (flags);
	}
