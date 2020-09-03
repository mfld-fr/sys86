// Lock-free byte queue
// for single reader & writer

#pragma once

#include "types.h"

#define QUEUE_SIZE 16
 
struct queue {
	word_t wr;  // next write
	word_t rd;  // next read
	byte_t buf [QUEUE_SIZE];
	};

void queue_init (struct queue * q);

int queue_not_empty (struct queue * q);
int queue_not_full (struct queue * q);

int queue_get (struct queue * q, byte_t * c);
int queue_put (struct queue * q, byte_t c);
