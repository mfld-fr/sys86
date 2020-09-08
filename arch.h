// SYS86 project
// Miscellaneous architecture specifics

#pragma once

#include "types.h"

#ifndef _ASSEMBLY

void halt (void);

word_t inw (word_t p);
void outw (word_t p, word_t w);

#endif // !_ASSEMBLY
