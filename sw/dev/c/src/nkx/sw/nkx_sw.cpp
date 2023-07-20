#include <stdint.h>

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

}; /* namespace nkx*/

