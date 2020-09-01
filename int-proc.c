#include "types.h"
#include "task.h"
#include "int.h"

void int_proc (word_t int_num, void * regs) {

	// Horrible hack to wake up receive task
	// in order to retry to peek key

	struct task * t = tasks [0];
	t->stat = TASK_RUN;
	task_sched ();
	}
