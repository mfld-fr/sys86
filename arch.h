#pragma once

#include "types.h"

void watchdog (void);

void halt (void);

word_t inw (word_t p);
void outw (word_t p, word_t w);
