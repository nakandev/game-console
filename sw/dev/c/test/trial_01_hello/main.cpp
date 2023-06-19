#include <nkx/hw/nkx_hw.h>
#include <cstdio>
#include <cstring>

int8_t gx[0x40] = {1,2,3,4,5,};
int8_t gy[0x80];

extern int main();
extern "C"
__attribute__((used, noreturn, section(".init")))
void _start()
{
  asm("li sp, 0x02010000");
  asm("call main");
  // main();
  while(1);
}

__attribute__((noinline))
int foo(int a, int b)
{
  return a + b;
}

int main()
{
  return foo(3, 5);
}
