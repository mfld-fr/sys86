
#include "lib.h"

static char_t hex [] = "0123456789ABCDEF";

void word_to_hex (word_t val, char_t * str, byte_t * len)
	{
	byte_t pos = 3;
	char_t buf [4];

	while (1) {
		buf [pos] = hex [val & 0x000F];
		val >>= 4;
		if (!val) break;
		pos--;
		}

	*len = 4 - pos;

	while (pos < 4) {
		*(str++) = buf [pos++];
		}
	}
