#include "hash.h"

uint32_t hash_pjw(const char *str)
{
	uint32_t val = 0, x;
	while (*str) {
		val = (val << 2) + *str;
		if (x = val & ~HASH_SIZE) val = (val ^ (x >> 12)) & HASH_SIZE;
		str ++;
	}
	return val;
}
