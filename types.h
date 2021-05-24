// SYS86 project
// Data types

#pragma once

#ifndef _ASSEMBLY

#include <stddef.h>

typedef unsigned char byte_t;
typedef unsigned short word_t;

typedef word_t reg_t;
typedef word_t seg_t;

typedef byte_t char_t;
typedef word_t err_t;

// Container helper

#define structof(_type, _member, _pointer) ( \
	(_type *) ((char *) _pointer - offsetof (_type, _member)))

#endif  // !_ASSEMBLY
