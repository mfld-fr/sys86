#pragma once

#include "types.h"

err_t recv_char (char_t * c);
err_t send_char (char_t c);

err_t send_string (char_t * s, word_t len);
