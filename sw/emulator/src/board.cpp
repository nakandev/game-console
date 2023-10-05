#include <board.h>
#include <fmt/core.h>

Board::Board()
: masterClock(30*1024*1024), memory(), io(memory), cpu(memory), ppu(memory), apu(memory),
  pause(true)
{
  io.setCpu(cpu);
  io.setPpu(ppu);
  io.setApu(apu);
}

Board::~Board()
{
}

void
Board::reset()
{
  cpu.init();
  ppu.init();
  apu.init();
  pause = false;
}

void Board::updateFrameUntilVblank()
{
  if (pause) return;
  for (int y=0; y<HW_SCREEN_H; y++) {
    for (int i=0; i<HW_SCREEN_W; i++) cpu.stepCycle();
    ppu.drawLine(y);
    if (y != HW_SCREEN_H - 1) {
      io.setIntStatus(HW_IO_INT_HBLANK);
      io.requestInt(HW_IO_INT_HBLANK);
      cpu.handleInterruption();
    }
    for (int i=0; i<HW_SCREEN_HBLANK; i++) cpu.stepCycle();
    if (y != HW_SCREEN_H - 1) {
      io.clearIntStatus(HW_IO_INT_HBLANK);
    }
    io.updateScanlineNumber(y);
  }
  apu.updateMusicBuffer();
}
void Board::updateFrameSinceVblank()
{
  if (pause) return;
  io.setIntStatus(HW_IO_INT_VBLANK);
  io.requestInt(HW_IO_INT_VBLANK);
  cpu.handleInterruption();
  for (int i=0; i<HW_SCREEN_VBLANK * (HW_SCREEN_W + HW_SCREEN_HBLANK); i++) cpu.stepCycle();
  io.clearIntStatus(HW_IO_INT_VBLANK);
}
