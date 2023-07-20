#include <nkx/hw/nkx_hw.h>
#include <cstdio>
#include <cstring>

extern uint32_t __init_array_start;
extern uint32_t __init_array_end;
extern int main();

extern "C" void random_op();

extern "C"
__attribute__((used, noreturn, section(".init")))
void _start()
{
  for(uint32_t p=__init_array_start; p<__init_array_end; p++) {
    (*(void(*)(void))p)();
  }
  main();
  while(1);
}

int main()
{
  random_op();
  return 0;
}
