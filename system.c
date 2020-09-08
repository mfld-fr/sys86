// SYS86 project
// Advantech SNMP-1000 system

#include "arch.h"
#include "io-dev.h"
#include "system.h"

// Quiet the watchdog / reset chip

void watchdog (void)
	{
	outw (IO_PORT0_DATA, inw (IO_PORT0_DATA) ^ IO_PORT0_WATCH);
	}

/*
void system_init (void)
	{
	}
*/
