// SYS86 project
// Serial port

#pragma once

#include "types.h"

#ifndef _ASSEMBLY

// Interrupt handler

void serial_proc (void);

// Operations

byte_t serial_get ();
void serial_put (byte_t c);

// Initialization

void serial_init (void);

#endif // !_ASSEMBLY
