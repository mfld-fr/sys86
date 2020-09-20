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

// Stack frame after interrupt entry
// Same offsets as in reg_s structure

// Both interrupted user & kernel
#define regs_dx     2*0x0
#define regs_cx     2*0x1
#define regs_bx     2*0x2
#define regs_es     2*0x3
#define regs_di     2*0x4
#define regs_si     2*0x5
#define regs_ax     2*0x6
#define regs_ds     2*0x7
#define regs_sp     2*0x8
#define regs_ss     2*0x9
// Only for interrupted kernel
#define regs_bp     2*0xA
#define regs_vect   2*0xB
#define regs_ip     2*0xC
#define regs_cs     2*0xD
#define regs_fl     2*0xE

#define REGS_KERN   2*0xA
#define REGS_USER   2*0x5
#define REGS_SIZE   2*0xF

#ifndef _ASSEMBLY

// Same offsets as defined by regs_* constants

struct regs_s {
	reg_t dx;
	reg_t cx;
	reg_t bx;
	reg_t es;
	reg_t di;
	reg_t si;
	reg_t ax;
	reg_t ds;
	seg_t ss;
	reg_t sp;
	reg_t bp;
	reg_t vect;
	reg_t ip;
	reg_t cs;
	reg_t fl;
	};

extern int int_level;

void vect_init ();

void int_proc (word_t vect);
void int_end (word_t vect);

//void int_enable (void);
word_t int_save (void);
void int_back (word_t saved);

void int_init (void);

#endif  // !_ASSEMBLY
