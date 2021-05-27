// SYS86 project
// R8810 timers

#include "types.h"
#include "arch.h"
#include "int.h"
#include "task.h"
#include "timer.h"
#include "board.h"

//------------------------------------------------------------------------------

static struct wait_s timer_wait;

// Wait timer

void timer_get ()
	{
	// NULL as condition test for pure event wait

	task_wait (&timer_wait, NULL, NULL, 1);
	}

//------------------------------------------------------------------------------

#ifdef CONFIG_TRACE

// Time sampling

struct trace_s traces [TRACE_MAX];
word_t trace_count = 0;
word_t trace_on = 0;

void trace_near (word_t num)
	{
	if (trace_on && (trace_count < TRACE_MAX)) {
		traces [trace_count].num = num;
		traces [trace_count].usec = inw (IO_TIMER2_COUNT);  // actually 0.2 us
		traces [trace_count].msec = inw (IO_TIMER0_COUNT);
		trace_count++;
		}
	}

#endif // CONFIG_TRACE

//------------------------------------------------------------------------------

// Interrupt procedure

void timer0_proc (void)
	{
	watchdog ();
	led_blink ();

	task_event (&timer_wait);
	}

// Timer initialization

void timer_init (void)
	{
	// Disable all timers

	outw (IO_TIMER0_MODE, 0x4000);
	outw (IO_TIMER1_MODE, 0x4000);
	outw (IO_TIMER2_MODE, 0x4000);

	// Interrupt setup

	int_set_timer ();

	// T0 timer @ 1 Hz
	// T2 timer @ 1 KHz

	outw (IO_TIMER2_COUNT, 0);
	outw (IO_TIMER0_COUNT, 0);

	outw (IO_TIMER2_MAX, 5000);
	outw (IO_TIMER0_MAX, 1000);

	outw (IO_TIMER2_MODE, 0xC001);
	outw (IO_TIMER0_MODE, 0xE009);

	// In case adjusting the timers
	// makes next timer interrupt too late
	// to quiet the HW watchdog

	watchdog ();
	}
