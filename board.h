// SYS86 project
// Advantech SNMP-1000 board

#pragma once

// Printer I/O ports

#define IO_PRINT_COM    0x031A

// GPIO ports

#define IO_PORT0_MASK   0xFE33
#define IO_PORT0_WATCH  0x0001

#define IO_PORT1_MASK   0x67F2
#define IO_PORT1_LED1   0x0002

//#define IO_PORT1_SERIAL_MASK 0x6700

#ifndef _ASSEMBLY

void watchdog (void);
void led_blink (void);

#endif // !_ASSEMBLY
