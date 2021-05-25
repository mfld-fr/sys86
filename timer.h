// SYS86 project
// Timers

#pragma once

#include "config.h"

#define IO_TIMER0_COUNT 0xFF50
#define IO_TIMER0_MAX   0xFF52
#define IO_TIMER0_MODE  0xFF56

#define IO_TIMER1_COUNT 0xFF58
#define IO_TIMER1_MAX   0xFF5A
#define IO_TIMER1_MODE  0xFF5E

#define IO_TIMER2_COUNT 0xFF60
#define IO_TIMER2_MAX   0xFF62
#define IO_TIMER2_MODE  0xFF66

#ifdef CONFIG_TRACE
#define TRACE_MAX 30
#endif // CONFIG_TRACE

#ifndef _ASSEMBLY

void timer0_proc (void);
void timer_init (void);
void timer_get (void);

#ifdef CONFIG_TRACE

struct trace_s {
	word_t num;
	word_t usec;
	word_t msec;
	word_t param;
};

extern struct trace_s traces [TRACE_MAX];
extern word_t trace_count;
extern word_t trace_on;

void trace_near (word_t num);
void trace_far (word_t num);

#endif // CONFIG_TRACE

#endif // !_ASSEMBLY
