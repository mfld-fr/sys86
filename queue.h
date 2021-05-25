// SYS86 project
// Lock-free byte queue for single reader / single writer

#pragma once

#include "types.h"


// Queue size has to be power of 2
// for easy calculation of R/W indexes

struct _queue_s
	{
	word_t wr;  // next write
	word_t rd;  // next read
	word_t sz;  // queue size
	byte_t * buf;
	};

typedef struct _queue_s queue_s;


void queue_init (queue_s * q, word_t size);

int queue_not_empty (queue_s * q);
int queue_not_full (queue_s * q);

byte_t queue_get (queue_s * q);
void queue_put (queue_s * q, byte_t c);

word_t queue_len (queue_s * q);

word_t queue_read (queue_s * q, byte_t * buf, word_t c1);
word_t queue_write (queue_s * q, byte_t * buf, word_t c1);
