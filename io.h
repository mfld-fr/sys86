#pragma once

#include "types.h"

// I/O registers

#define io_print_com    0x031A

#define IO_INT_EOI         0xFF22

#define io_timer0_mode  0xFF56
#define io_timer1_mode  0xFF5E
#define io_timer2_mode  0xFF66

#define io_port0_data   0xFF74

#define io_port1_mode   0xFF76
#define io_port1_dir    0xFF78
#define io_port1_data   0xFF7A

// PIO masks

#define io_port0_mask         0xFE33
#define io_port1_mask         0x67F2

#define io_port1_led1_mask    0x0002
#define io_port1_serial_mask  0x6700

#ifndef _ASSEMBLY

word_t inw (word_t p);
void outw (word_t p, word_t w);

#endif // !_ASSEMBLY


