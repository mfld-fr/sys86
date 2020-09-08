#pragma once

#include "types.h"

#ifndef _ASSEMBLY

// Interrupt handler

void serial_proc (void);

// Operations

int serial_read (byte_t * c);
void serial_send (byte_t c);

// Initialization

void serial_init (void);

#endif // !_ASSEMBLY
