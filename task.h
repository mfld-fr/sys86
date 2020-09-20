// SYS86 project
// Task management

#pragma once

#include "types.h"

#define TASK_DOWN 0
#define TASK_RUN  1
#define TASK_WAIT 2

#define STACK_SIZE 256

#define TASK_MAX 4

// Offsets in task_s structure

#define TOP_SP     2*0
#define task_level 2*1
#define task_stack 2*2
#define task_ssize 2*3

#ifndef _ASSEMBLY

// Forward declaration

struct task_s;

// Wait structure
// Very simplified wait queue :-)

struct wait_s
	{
	struct task_s * t;
	};

// Task structure

struct task_s
	{
	// Shared with assembly

	reg_t top_sp;    // 0
	word_t level;    // 1
	word_t * stack;  // 2
	word_t ssize;    // 3

	// C only

	int stat;
	struct wait_s * wait;  // object waited for
	};

extern struct task_s * tasks [TASK_MAX];

extern struct task_s * task_prev;
extern struct task_s * task_cur;
extern struct task_s * task_next;

extern int sched_lock;
extern int sched_need;

// From assembly

void stack_init_near (struct task_s *, void * entry, word_t * stack);
void stack_init_far (struct task_s *, word_t ip, word_t cs, word_t sp, word_t ss, word_t * stack);

void task_switch (void);

// From C code

void task_init_near (int i, struct task_s * t, void * entry, word_t * stack, word_t size);
void task_init_far (int i, struct task_s * t, word_t seg, word_t * stack, word_t size);

void task_sched (void);

typedef int (* cond_f) (void * object);
void task_wait (struct wait_s * wait, cond_f test, void * param, int single);
void task_event (struct wait_s * wait);

void task_init (void);

#endif // !_ASSEMBLY
