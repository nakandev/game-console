#pragma once
#include <cstdint>

#define arraysizeof(x) (sizeof(x)/sizeof(x[0]))

extern int debugLevel;

typedef union {
  int32_t s;
  uint32_t u;
  float f;
} union32_t;

inline int32_t sext(int sb, uint32_t v)
{
  return (v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0);
}

