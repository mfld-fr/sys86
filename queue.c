// SYS86 project
// Lock-free byte queue for single reader / single writer

#include "queue.h"
#include "heap.h"


void queue_init (queue_s * q, word_t s)
	{
	q->wr = 0;
	q->rd = 0;
	q->sz = s;
	q->buf = heap_alloc (s, HEAP_TAG_QUEUE);
	}


int queue_not_empty (queue_s * q)
	{
	if (q->rd == q->wr) return 0;
	return 1;
	}

byte_t queue_get (queue_s * q)
	{
	byte_t c = q->buf [q->rd];
	q->rd = (q->rd + 1) & (q->sz - 1);
	return c;
	}

int queue_not_full (queue_s * q)
	{
	if (((q->wr + 1) & (q->sz - 1)) == q->rd) return 0;
	return 1;
	}

void queue_put (queue_s * q, byte_t c)
	{
	q->buf [q->wr] = c;
	q->wr = (q->wr + 1) & (q->sz - 1);
	}

word_t queue_len (queue_s * q)
	{
	return (q->wr - q->rd) & (q->sz - 1);
	}

word_t queue_read (queue_s * q, byte_t * buf, word_t c1)
	{
	word_t c2 = 0;
	word_t r1 = q->rd;

	while (c2 < c1)
		{
		if (r1 == q->wr) break;
		buf [c2++] = q->buf [r1];
		r1 = (r1 + 1) & (q->sz - 1);
		}

	q->rd = r1;
	return c2;
	}

word_t queue_write (queue_s * q, byte_t * buf, word_t c1)
	{
	word_t c2 = 0;
	word_t w1 = q->wr;

	while (c2 < c1)
		{
		word_t w2 = (w1 + 1) & (q->sz - 1);
		if (w2 == q->rd) break;
		q->buf [w1] = buf [c2++];
		w1 = w2;
		}

	q->wr = w1;
	return c2;
	}
