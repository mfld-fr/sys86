#pragma once

#define TASK_DOWN 0
#define TASK_RUN  1
#define TASK_WAIT 2

#define task_stack_size 256

#define TASK_MAX 3

#ifndef _ASSEMBLY

// Task structure

// TODO: move the stack out of the task object
// to allow stack out the kernel data segment

struct task {

	// Architecture specific

	reg_t sp;
	word_t level;
	word_t stack [task_stack_size];

	// Generic

	int stat;
	};

extern struct task * tasks [TASK_MAX];
extern struct task * task_cur;

#endif // !_ASSEMBLY

#define task_sp    0x00
#define task_level 0x02
#define task_stack 0x04

#ifndef _ASSEMBLY


void task_init_kern (struct task *, void * entry);

void task_switch (struct task * prev, struct task * next);

void task_sched ();

#endif // !_ASSEMBLY
