#include <timer.h>
#include <memory.h>
#include <intrrctrl.h>
#include <fmt/core.h>

Timer::Timer(Memory& memory, IntrrCtrl& intrrCtrl)
  : memory(memory), intrrCtrl(intrrCtrl), ioram()
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

void Timer::stepCycle()
{
}
