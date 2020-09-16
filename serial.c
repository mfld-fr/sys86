// SYS86 project
// R8810 serial port

#include "task.h"
#include "queue.h"
#include "serial.h"
#include "arch.h"
#include "timer.h"

// R8810 serial port I/O

#define IO_SERIAL_CONTROL  0xFF80
#define IO_SERIAL_STATUS   0xFF82
#define IO_SERIAL_TDATA    0xFF84
#define IO_SERIAL_RDATA    0xFF86

// R8810 serial registers

#define SERIAL_CONTROL_RIE 0x0400  // receive interrupt enable

#define SERIAL_STATUS_TDR  0x0020  // transmit data ready
#define SERIAL_STATUS_RDR  0x0010  // receive data ready

// Generic I/O queues

static struct queue_s serial_in;
static struct wait_s serial_wait;

// Interrupt procedure

void serial_proc (void)
	{
	word_t stat = inw (IO_SERIAL_STATUS);

	if (stat & SERIAL_STATUS_RDR) {
		word_t c = inw (IO_SERIAL_RDATA);
		time_sample (1);
		if (queue_not_full (&serial_in)) {
			queue_put (&serial_in, (byte_t) c);
			task_event (&serial_wait);
			}
		}
	}

byte_t serial_read (void)
	{
	task_wait (&serial_wait, (cond_f) queue_not_empty, &serial_in, 1);
	return queue_get (&serial_in);
	}

void serial_send (byte_t c)
	{
	while (!(inw (IO_SERIAL_STATUS) & SERIAL_STATUS_TDR));
	outw (IO_SERIAL_TDATA, (word_t) c);
	}

void serial_init (void)
	{
	// Enable serial receive interrupt

	outw (IO_SERIAL_CONTROL, SERIAL_CONTROL_RIE);
	}
