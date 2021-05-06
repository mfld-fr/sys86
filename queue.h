// SYS86 project
// Lock-free byte queue for single reader / single writer

#pragma once

#include "types.h"

// Queue size has to be power of 2
// for easy calculation of R/W indexes

#define QUEUE_SIZE 16
 
struct queue_s
	{
	word_t wr;  // next write
	word_t rd;  // next read
	byte_t buf [QUEUE_SIZE];
	};

// TODO: allocate queue buffer on heap
// void queue_init (struct queue_s * q);

int queue_not_empty (struct queue_s * q);
int queue_not_full (struct queue_s * q);

byte_t queue_get (struct queue_s * q);
void queue_put (struct queue_s * q, byte_t c);

word_t queue_len (struct queue_s * q);

word_t queue_read (struct queue_s * q, byte_t * buf, word_t c1);
word_t queue_write (struct queue_s * q, byte_t * buf, word_t c1);
