#pragma once

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

#define IO_SERIAL_CONTROL  0xFF80
#define IO_SERIAL_STATUS   0xFF82
#define IO_SERIAL_TDATA    0xFF84
#define IO_SERIAL_RDATA    0xFF86

// PIO masks

#define io_port0_mask         0xFE33
#define io_port1_mask         0x67F2

#define io_port1_led1_mask    0x0002
#define io_port1_serial_mask  0x6700

// Serial registers

#define SERIAL_CONTROL_RIE  0x0400  // receive interrupt enable

#define SERIAL_STATUS_TDR   0x0020  // transmit data ready
#define SERIAL_STATUS_RDR   0x0010  // receive data ready
