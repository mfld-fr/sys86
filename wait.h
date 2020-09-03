#pragma once

#ifndef _ASSEMBLY

enum event_e {
	EVENT_NONE = 0,
	EVENT_QUEUE_NOT_FULL,
	EVENT_QUEUE_NOT_EMPTY,
	EVENT_CONSOLE_IN
	};

void task_event (int event);

typedef int (* cond_f) (void * param);
void event_wait (int event, cond_f test, void * param);

#endif // !_ASSEMBLY
