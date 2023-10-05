#include <stdint.h>
#include <memorymap.h>
#include <nkx/sw/nkx_sw.h>

namespace nkx {

static const int32_t _randvalue1 = 1234567;
static const int32_t _randvalue2 = 1357;
static int32_t _randvalue = 0;

__attribute__((noinline))
void srand(int32_t seed)
{
  _randvalue = seed;
}

__attribute__((noinline))
int32_t rand()
{
  _randvalue *= _randvalue1;
  _randvalue += _randvalue2;
  return _randvalue;
}

/* standard functions */
uint8_t strlen(const char* str, uint8_t n)
{
  int i = 0;
  for (i=0; i<n; i++) {
    if (str[i] == '\0') return i;
  }
  return i;
}
}; /* namespace nkx*/

