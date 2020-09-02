#pragma once

#define TASK_DOWN 0
#define TASK_RUN  1
#define TASK_WAIT 2

#define STACK_SIZE 256

#define TASK_MAX 3

#ifndef _ASSEMBLY

// Task structure
// TODO: stack size member to adjust per task

struct task_s {

	// Shared with assembly

	reg_t sp;
	word_t level;
	word_t * stack;
	word_t ssize;

	// C only

	int stat;
	};

extern struct task_s * tasks [TASK_MAX];
extern struct task_s * task_cur;

#endif // !_ASSEMBLY

// Offsets in task_s structure

#define task_sp    0x00
#define task_level 0x02
#define task_stack 0x04
#define task_ssize 0x06

#ifndef _ASSEMBLY


void task_init_kern (struct task_s *, void * entry);

void task_switch (struct task_s * prev, struct task_s * next);

void task_sched ();

#endif // !_ASSEMBLY
