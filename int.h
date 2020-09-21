// SYS86 project
// Interrupt handling

#pragma once

#include "config.h"
#include "types.h"

// Hardware interrupt vectors
// R8810 specific
// TODO: move to int-dev.h

#define VECT_TIMER0 0x08
#define VECT_SERIAL 0x14

// System call vector with kernel CS

#define VECT_SYSTEM 0x80

// Global vector with kernel DS

#define VECT_GLOBAL 0xFF

#ifdef CONFIG_INT_USER
#include "int-user.h"
#endif // CONFIG_INT_USER

#ifdef CONFIG_INT_KERN
#include "int-kern.h"
#endif // CONFIG_INT_KERN

#ifndef _ASSEMBLY

extern int int_level;

void vect_init ();

void int_proc (word_t vect);
void int_end (word_t vect);

//void int_enable (void);
word_t int_save (void);
void int_back (word_t saved);

void int_init (void);

#endif  // !_ASSEMBLY
