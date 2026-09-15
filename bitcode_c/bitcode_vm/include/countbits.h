#include <stdint.h>

#ifndef COUNTBITS_H_
#define COUNTBITS_H_

extern uint64_t count_one_bits64(uint64_t bits);
extern uint64_t count_zero_bits64(uint64_t bits);

extern uint32_t count_one_bits32(uint32_t bits);
extern uint32_t count_zero_bits32(uint32_t bits);
#endif

