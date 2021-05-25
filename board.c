// SYS86 project
// Advantech SNMP-1000 board

#include "arch.h"
#include "io-dev.h"
#include "board.h"

// Quiet the external watchdog chip
// every second to avoid system reset

void watchdog (void)
	{
	outw (IO_PORT0_DATA, inw (IO_PORT0_DATA) ^ IO_PORT0_WATCH);
	}

// Toggle LED1

void led_blink (void)
	{
	outw (IO_PORT1_DATA, inw (IO_PORT1_DATA) ^ IO_PORT1_LED1);
	}
