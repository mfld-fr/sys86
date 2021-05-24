
#include "lib.h"

static char_t hex [] = "0123456789ABCDEF";

byte_t word_to_hex (word_t val, char_t * str)
	{
	byte_t pos = 0;
	byte_t f = 0;
	word_t i;

	if ((i = val & 0xF000)) {
		str [pos++] = hex [i >> 12];
		f = 1;
		}

	if ((i = val & 0x0F00) || f) {
		str [pos++] = hex [i >> 8];
		f = 1;
		}

	if ((i = val & 0x00F0) || f) {
		str [pos++] = hex [i >> 4];
		}

	str [pos++] = hex [val & 0x000F];
	return pos;
	}
