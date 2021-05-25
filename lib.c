
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


static word_t dec [] = { 10000, 1000, 100, 10 };

byte_t word_to_dec (word_t val, char_t * str)
	{
	byte_t pos = 0;
	byte_t f = 0;
	word_t i = 0;

	while (i < 4) {
		byte_t n = 0;
		while (val >= dec [i]) {
			val -= dec [i];
			n++;
			f = 1;
			}

		if (f) str [pos++] = (char_t) (n + '0');
		i++;
		}

	str [pos++] = (char_t) (val + '0');
	return pos;
	}
