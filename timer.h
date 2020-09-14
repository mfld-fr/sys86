// SYS86 project
// Timers

#pragma once

#define IO_TIMER0_COUNT 0xFF50
#define IO_TIMER0_MAX   0xFF52
#define IO_TIMER0_MODE  0xFF56

#define IO_TIMER1_COUNT 0xFF58
#define IO_TIMER1_MAX   0xFF5A
#define IO_TIMER1_MODE  0xFF5E

#define IO_TIMER2_COUNT 0xFF60
#define IO_TIMER2_MAX   0xFF62
#define IO_TIMER2_MODE  0xFF66

#ifndef _ASSEMBLY

void timer0_proc (void);
void timer_init (void);

#define SAMPLE_MAX 10
extern word_t time_samples [SAMPLE_MAX];

void time_sample (word_t index);
word_t time_diff (word_t begin, word_t end);

#endif // !_ASSEMBLY
