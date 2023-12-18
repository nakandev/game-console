#include <intrrctrl.h>
#include <memory.h>
#include <cpu.h>

IntrrCtrl::IntrrCtrl(Memory& memory, Cpu& cpu)
: memory(memory), cpu(cpu)
{
}

IntrrCtrl::~IntrrCtrl()
{
}

void IntrrCtrl::requestInt(uint8_t intno)
{
  uint32_t enable = memory.read32(HWREG_IO_INT_ENABLE_ADDR);
  uint32_t status = memory.read32(HWREG_IO_INT_STATUS_ADDR);
  if ((enable & (1u << intno)) && (status & (1u << intno))) {
    cpu.requestInterruption();
    cpu.handleInterruption();
  }
}

void IntrrCtrl::setIntStatus(uint8_t intno)
{
  uint32_t status = memory.read32(HWREG_IO_INT_STATUS_ADDR);
  status |= (1u << intno);
  memory.write32(HWREG_IO_INT_STATUS_ADDR, status);
}

void IntrrCtrl::clearIntStatus(uint8_t intno)
{
  uint32_t status = memory.read32(HWREG_IO_INT_STATUS_ADDR);
  status &= ~(1u << intno);
  memory.write32(HWREG_IO_INT_STATUS_ADDR, status);
}
