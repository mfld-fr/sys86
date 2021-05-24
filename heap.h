// Local heap

#pragma once

#include "types.h"
#include "list.h"

/*#define HEAP_DEBUG*/

// Heap block header

// Tag bits 0-6 help for statistics

#define HEAP_TAG_FREE    0x00
#define HEAP_TAG_USED    0x80
#define HEAP_TAG_TASK    0x01


// TODO: move free list node from header to body
// to reduce overhead for allocated block

struct _heap_s {
	list_s all;
	list_s free;
	word_t size;
	byte_t tag;
	byte_t unused;  // padding
} __attribute__ ((packed));

typedef struct _heap_s heap_s;

// Heap functions

void * heap_alloc (word_t size, byte_t tag);
void heap_free (void * data);

void heap_add (void * data, word_t size);
void heap_init ();

#ifdef HEAP_DEBUG
void heap_iterate (void (* cb) (heap_s * h));
#endif /* HEAP_DEBUG */