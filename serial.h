// SYS86 project
// Serial port

#pragma once

#include "types.h"

#ifndef _ASSEMBLY

// Interrupt handler

void serial_proc (void);

// Operations

void serial_send (byte_t c);

byte_t serial_get ();

// Initialization

void serial_init (void);

#endif // !_ASSEMBLY
