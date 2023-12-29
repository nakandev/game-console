#include <timer.h>
#include <memory.h>
#include <intrrctrl.h>
#include <fmt/core.h>

Timer::Timer(Memory& memory, IntrrCtrl& intrrCtrl)
: MemorySection("timer", HWREG_IO_TIMER_ADDR, 0x100),
  memory(memory), intrrCtrl(intrrCtrl), channels()
{
  channels.resize(4);
  init();
}

Timer::~Timer()
{
}

auto Timer::init() -> void
{
  memory.addSection<Timer>(this);
  HwIoTimer& timer = *(HwIoTimer*)this->data.data();

  for (auto& tm: timer.timer) {
    tm.count = 0;
    tm.enable = false;
  }
}

auto Timer::stepCycle() -> void
{
  HwIoTimer& timer = *(HwIoTimer*)this->data.data();
  for (int chIdx=0; chIdx<4; chIdx++) {
    auto& tm = timer.timer[chIdx];
    if (!tm.enable) continue;
    tm.count++;
    if (tm.count == tm.limit) {
      if (tm.interrupt) {
        uint32_t intno = HW_IO_INT_TIMER0 + chIdx;
        intrrCtrl.setIntStatus(intno);
        intrrCtrl.requestInt(intno);
      }
      tm.count = 0;
      if (!tm.repeat) {
        tm.enable = false;
      }
    }
  }
}

auto Timer::write8(uint32_t addr, int8_t value) -> void
{
  MemorySection::write8(addr, value);
}

auto Timer::write16(uint32_t addr, int16_t value) -> void
{
  MemorySection::write16(addr, value);
}

auto Timer::write32(uint32_t addr, int32_t value) -> void
{
  MemorySection::write32(addr, value);
}

auto Timer::write(uint32_t addr, uint32_t size, int32_t value) -> void
{
  MemorySection::write(addr, size, value);
}
