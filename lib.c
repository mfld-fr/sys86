
#include "lib.h"

char_t digit_to_hex (byte_t d)
	{
	return (d > 9) ? 'A' + d - 10 : '0' + d;
	}

void word_to_hex (word_t val, char_t * str, byte_t * len)
	{
	byte_t pos = 3;
	char_t buf [4];

	while (1) {
		buf [pos] = digit_to_hex ((byte_t) val & 0x000F);
		val >>= 4;
		if (!val) break;
		pos--;
		}

	*len = 4 - pos;

	while (pos < 4) {
		*(str++) = buf [pos++];
		}
	}
