// EMU86 project
// R8810 timers

#include "types.h"
#include "arch.h"
#include "timer.h"
#include "system.h"

// Interrupt procedure

void timer0_proc (void)
	{
	watchdog ();
	led_blink ();
	}

// Time sampling
// Use the T1 timer @ 5 MHz

word_t time_samples [SAMPLE_MAX];

void time_sample (word_t index)
	{
	time_samples [index] = inw (IO_TIMER1_COUNT);
	}

word_t time_diff (word_t begin, word_t end)
	{
	return (time_samples [end] - time_samples [begin]);
	}

// Timer initialization

void timer_init (void)
	{
	// Disable T1 timer

	word_t t1m = inw (IO_TIMER1_MODE);
	t1m &= 0x7FFF;  // disable timer bit
	t1m |= 0x4000;  // unlock enable bit
	outw (IO_TIMER1_MODE, t1m);

	// Change T1 max count

	outw (IO_TIMER1_MAX, 0xFFFF);

	// Enable T1 back

	outw (IO_TIMER1_MODE, 0xC001);

	// T0 timer @ 1 Hz
	// T2 timer @ 1 KHz

	//outw (IO_TIMER0_MAX, 1000);
	//outw (IO_TIMER2_MAX, 5000);
	}
