#ifndef _hash_h_
#define _hash_h_

#include <stdint.h>

#define HASH_SIZE 0x3fff

uint32_t hash_pjw(const char *str);
#define hash(x) hash_pjw(x)

#endif
