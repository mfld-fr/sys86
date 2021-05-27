// SYS86 project
// Local heap

#pragma once

#include "config.h"
#include "types.h"
#include "list.h"


// Heap block header

// Tag bits 0-6 help for statistics

#define HEAP_TAG_FREE    0x00
#define HEAP_TAG_USED    0x80
#define HEAP_TAG_STACK   0x01
#define HEAP_TAG_QUEUE   0x02

// TODO: move free list node from header to body
// to reduce overhead for allocated block

struct _heap_s {
	list_s all;
	list_s free;
	word_t size;
	byte_t tag;
} __attribute__ ((packed));

typedef struct _heap_s heap_s;

// Heap functions

void * heap_alloc (word_t size, byte_t tag);
void heap_free (void * data);

void heap_add (void * data, word_t size);
void heap_init ();

#ifdef HEAP_DEBUG
void heap_iterate (void (* cb) (heap_s * h));
#endif  // HEAP_DEBUG
