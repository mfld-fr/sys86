// SYS86 project
// Lock-free byte queue for single reader / single writer

#pragma once

#include "types.h"

#define QUEUE_SIZE 16
 
struct queue_s
	{
	word_t wr;  // next write
	word_t rd;  // next read
	byte_t buf [QUEUE_SIZE];
	};

//void queue_init (struct queue_s * q);

int queue_not_empty (struct queue_s * q);
int queue_not_full (struct queue_s * q);

byte_t queue_get (struct queue_s * q);
void queue_put (struct queue_s * q, byte_t c);
