// SYS86 project
// Interrupt handling
// Context on user stack

#pragma once

#include "types.h"

// Stack frame after interrupt entry
// Same offsets as in reg_s structure

#define regs_di     2*0x0
#define regs_si     2*0x1
#define regs_bp     2*0x2
#define regs_bx     2*0x3
#define regs_dx     2*0x4
#define regs_cx     2*0x5
#define regs_ax     2*0x6
#define regs_ds     2*0x7
#define regs_es     2*0x8
#define regs_num    2*0x9
#define regs_ip     2*0xA
#define regs_cs     2*0xB
#define regs_fl     2*0xC

#define REGS_SIZE   2*0xD

#ifndef _ASSEMBLY

// Same offsets as defined by regs_* constants

struct regs_s {
	reg_t di;
	reg_t si;
	reg_t bp;
	reg_t bx;
	reg_t dx;
	reg_t cx;
	reg_t ax;
	reg_t ds;
	reg_t es;
	reg_t num;
	reg_t ip;
	reg_t cs;
	reg_t fl;
	};

#endif  // !_ASSEMBLY

