// SYS86 project
// Lock-free byte queue for single reader / single writer

#include "queue.h"

/*
void queue_init (struct queue_s * q)
	{
	q->wr = 0;
	q->rd = 0;
	}
*/

int queue_not_empty (struct queue_s * q)
	{
	if (q->rd == q->wr) return 0;
	return 1;
	}

byte_t queue_get (struct queue_s * q)
	{
	byte_t c = q->buf [q->rd];
	q->rd = (q->rd + 1) % QUEUE_SIZE;
	return c;
	}

int queue_not_full (struct queue_s * q)
	{
	if ((q->wr + 1) % QUEUE_SIZE == q->rd) return 0;
	return 1;
	}

void queue_put (struct queue_s * q, byte_t c)
	{
	q->buf [q->wr] = c;
	q->wr = (q->wr + 1) % QUEUE_SIZE;
	}
