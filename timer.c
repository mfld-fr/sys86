// EMU86 project
// R8810 timers

#include "types.h"
#include "arch.h"
#include "timer.h"
#include "system.h"

#define IO_TIMER0_MODE 0xFF56
#define IO_TIMER1_MODE 0xFF5E
#define IO_TIMER2_MODE 0xFF66

// Interrupt procedure

void timer0_proc (void)
	{
	// TODO: rewrite in C
	watchdog ();
	}

void timer_init (void)
	{
	// Disable unused T1 timer @ 1000 Hz to save power

	word_t t1m = inw (IO_TIMER1_MODE);
	t1m &= 0x7FFF;  // disable timer bit
	t1m |= 0x4000;  // unlock enable bit
	outw (IO_TIMER1_MODE, t1m);
	}
