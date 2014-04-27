#include <stdlib.h>

#include "strtool.h"

/*------------------------------------------------------------------------*/

void strrand(char* s, size_t len)
{
	const char dict[] =
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789"
		"{}()<>"
		"*-+~";

	if(0 == len) {
		return;
	}

	while(len > 1) {
		*s = dict[rand() % (ARRAY_SIZE(dict) - 1)];

		++ s;
		-- len;
	}

	*s = 0;
}
