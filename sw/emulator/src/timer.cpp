#include <timer.h>
#include <memory.h>
#include <fmt/core.h>

Timer::Timer(Memory& memory)
  : memory(memory)
{
  ioram = (HwIoRam*)memory.section("ioram").buffer();
  init();
}

Timer::~Timer()
{
}

void Timer::init()
{
}

void Timer::stepCpuCycle()
{
}
