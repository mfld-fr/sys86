// SYS86 project
// R8810 serial port

#include "task.h"
#include "queue.h"
#include "serial.h"
#include "arch.h"
#include "timer.h"

// R8810 serial port I/O

#define IO_SERIAL_MODE     0xFF80
#define IO_SERIAL_STATUS   0xFF82
#define IO_SERIAL_TDATA    0xFF84
#define IO_SERIAL_RDATA    0xFF86

// R8810 serial registers

#define SERIAL_MODE_TXIE 0x0400  // transmit interrupt enable
#define SERIAL_MODE_RXIE 0x0400  // receive interrupt enable

#define SERIAL_STATUS_THRE  0x0020  // transmit holding register empty
#define SERIAL_STATUS_RDR   0x0010  // receive data ready

// Serial I/O queues

static queue_s serial_qin;
static struct wait_s serial_win;

static queue_s serial_qout;
static struct wait_s serial_wout;

static struct wait_s serial_wait;


// Interrupt procedure

void serial_proc (void)
	{
	word_t stat = inw (IO_SERIAL_STATUS);

	if (stat & SERIAL_STATUS_RDR) {
		word_t c = inw (IO_SERIAL_RDATA);
		if (queue_not_full (&serial_qin)) {
			queue_put (&serial_qin, (byte_t) c);
			task_event (&serial_win);
			}
		}

	if (stat & SERIAL_STATUS_THRE) {
		task_event (&serial_wait);
		}
	}


byte_t serial_get (void)
	{
	task_wait (&serial_win, (cond_f) queue_not_empty, &serial_qin, 1);
	return queue_get (&serial_qin);
	}

void serial_put (byte_t c)
	{
	task_wait (&serial_wout, (cond_f) queue_not_full, &serial_qout, 1);
	queue_put (&serial_qout, c);
	task_event (&serial_wout);
	}


static int serial_ready (void)
	{
	return (inw (IO_SERIAL_STATUS) & SERIAL_STATUS_THRE);
	}

static struct task_s serial_task;

static void serial_main (void)
	{
	while (1)
		{
		// Wait for data in queue

		task_wait (&serial_wout, (cond_f) queue_not_empty, &serial_qout, 1);
		byte_t c = queue_get (&serial_qout);
		task_event (&serial_wout);  // not more full

		task_wait (&serial_wait, (cond_f) serial_ready, NULL, 1);
		outw (IO_SERIAL_TDATA, (word_t) c);
		}
	}


void serial_init (void)
	{
	// Prepare I/O queues

	queue_init (&serial_qin,  SERIAL_QUEUE_SIZE);
	queue_init (&serial_qout, SERIAL_QUEUE_SIZE);

	// Enable serial receive & transmit interrupts

	word_t mode = inw (IO_SERIAL_MODE);
	mode |= SERIAL_MODE_RXIE | SERIAL_MODE_TXIE;
	outw (IO_SERIAL_MODE, mode);

	// Start serial task

	task_init_near (TASK_NUM_SERIAL, &serial_task, serial_main, STACK_SIZE);
	}
