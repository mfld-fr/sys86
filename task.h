#pragma once

#define TASK_DOWN 0
#define TASK_RUN  1
#define TASK_WAIT 2

#define STACK_SIZE 256

#define TASK_MAX 3

// Offsets in task_s structure

#define KERN_SP    0x00
#define task_level 0x02
#define task_stack 0x04
#define task_ssize 0x06
#define USER_SP    0x08
#define USER_SS    0x0A

#ifndef _ASSEMBLY

// Task structure

struct task_s {

	// Shared with assembly

	reg_t kern_sp;
	word_t level;
	word_t * stack;
	word_t ssize;
	reg_t user_sp;
	seg_t user_ss;

	// C only

	int stat;
	int wait;
	};

extern struct task_s * tasks [TASK_MAX];
extern struct task_s * task_cur;

extern int sched_lock;
extern int sched_need;

void task_init_kern (struct task_s *, void * entry);
void task_switch (struct task_s * prev, struct task_s * next);

void schedule ();

#endif // !_ASSEMBLY
