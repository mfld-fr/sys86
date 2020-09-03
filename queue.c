// Lock-free byte queue
// for single reader & writer

#include "queue.h"

void queue_init (struct queue * q) {
	q->wr = 0;
	q->rd = 0;
	}

int queue_not_empty (struct queue * q) {
	if (q->rd == q->wr) return 0;
	return 1;
	}

int queue_get (struct queue * q, byte_t * c) {
	if (!queue_not_empty (q)) return 1;
	*c = q->buf [q->rd];
	q->rd = (q->rd + 1) % QUEUE_SIZE;
	return 0;
	}

int queue_not_full (struct queue * q) {
	if ((q->wr + 1) % QUEUE_SIZE == q->rd) return 0;
	return 1;
	}

int queue_put (struct queue * q, byte_t c) {
	if (!queue_not_full (q)) return 1;
	q->buf [q->wr] = c;
	q->wr = (q->wr + 1) % QUEUE_SIZE;
	return 0;
	}
