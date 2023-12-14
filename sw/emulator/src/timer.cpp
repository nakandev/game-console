#include <timer.h>
#include <memory.h>
#include <io.h>
#include <fmt/core.h>

Timer::Timer(Memory& memory, IO& io)
  : memory(memory), io(io), ioram()
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
