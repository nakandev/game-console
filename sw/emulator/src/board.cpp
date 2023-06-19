#include <board.h>

Board::Board()
  : memory(), io(memory), cpu(memory), ppu(memory)
{
  io.setCpu(cpu);
  io.setPpu(ppu);
}

Board::~Board()
{
}

enum {
  SC_W = 320,
  SC_H = 240,
  BLANK_W = 80,
  BLANK_H = 80,
};
void Board::updateFrameUntilVblank()
{
  for (int y=0; y<SC_H; y++) {
    for (int i=0; i<SC_W; i++) cpu.stepi();
    ppu.drawLine(y);
    io.setExtIntStatus(IO_EXTINT_HBLANK);
    io.requestExtInt(IO_EXTINT_HBLANK);
    for (int i=0; i<BLANK_W; i++) cpu.stepi();
    io.clearExtIntStatus(IO_EXTINT_HBLANK);
  }
}
void Board::updateFrameSinceVblank()
{
  io.setExtIntStatus(IO_EXTINT_VBLANK);
  io.requestExtInt(IO_EXTINT_VBLANK);
  for (int i=0; i<BLANK_H * (SC_W + BLANK_W); i++) cpu.stepi();
  io.clearExtIntStatus(IO_EXTINT_VBLANK);
}
