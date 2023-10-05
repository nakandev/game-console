#include <cstdint>

extern uint32_t __init_array_start;
extern uint32_t __init_array_end;
extern int main();
extern "C" {
extern void _start();
__attribute__((used, section(".init")))
void __init_array()
{
  for(uint32_t p=__init_array_start; p<__init_array_end; p++) {
    (*(void(*)(void))p)();
  }
}
} /* extern "C" */
