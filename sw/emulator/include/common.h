#pragma once
#include <cstdint>

#define arraysizeof(x) (sizeof(x)/sizeof(x[0]))

extern int debugLevel;

union bool8_t {
  bool v1;
  uint8_t v8;
};

union union32_t {
  int32_t s;
  uint32_t u;
  float f;
};

inline auto sext(int sb, uint32_t v) -> int32_t
{
  return (v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0);
}

