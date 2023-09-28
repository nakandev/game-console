#include <board.h>
#include <fmt/core.h>

Board::Board()
  : masterClock(30*1024*1024), memory(), io(memory), cpu(memory), ppu(memory), apu(memory)
{
  io.setCpu(cpu);
  io.setPpu(ppu);
  io.setApu(apu);
}

Board::~Board()
{
}

void Board::updateFrameUntilVblank()
{
  for (int y=0; y<HW_SCREEN_H; y++) {
    for (int i=0; i<HW_SCREEN_W; i++) cpu.stepi();
    ppu.drawLine(y);
    io.setExtIntStatus(HW_IO_EXTINT_HBLANK);
    io.requestExtInt(HW_IO_EXTINT_HBLANK);
    cpu.handleInterruption();
    for (int i=0; i<HW_SCREEN_HBLANK; i++) cpu.stepi();
    io.clearExtIntStatus(HW_IO_EXTINT_HBLANK);
  }
  apu.updateMusicBuffer();
}
void Board::updateFrameSinceVblank()
{
  io.setExtIntStatus(HW_IO_EXTINT_VBLANK);
  io.requestExtInt(HW_IO_EXTINT_VBLANK);
  cpu.handleInterruption();
  for (int i=0; i<HW_SCREEN_VBLANK * (HW_SCREEN_W + HW_SCREEN_HBLANK); i++) cpu.stepi();
  io.clearExtIntStatus(HW_IO_EXTINT_VBLANK);
}
