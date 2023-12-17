#include <board.h>
#include <fmt/core.h>

Board::Board()
: masterClock(30*1024*1024),
  memory(),
  io(memory),
  cpu(memory),
  vpu(memory),
  apu(memory),
  dma(memory, io),
  timer(memory, io),
  pause(true)
{
  io.setCpu(cpu);
  io.setVpu(vpu);
  io.setApu(apu);
  io.setDma(dma);
  io.setTimer(timer);

  cpu.init();
  vpu.init();
  apu.init();
  dma.init();
  timer.init();
}

Board::~Board()
{
}

void
Board::reset()
{
  cpu.init();
  vpu.init();
  apu.init();
  dma.init();
  timer.init();
  pause = false;
}

void Board::updateFrameUntilVblank()
{
  if (pause) return;
  for (int y=0; y<HW_SCREEN_H; y++) {
    for (int i=0; i<HW_SCREEN_W*HW_VPU_CPUCYCLE_RATIO; i++) {
      stepCpuCycle();
    }
    vpu.drawLine(y);
    if (y != HW_SCREEN_H - 1) {
      io.setIntStatus(HW_IO_INT_HBLANK);
      io.requestInt(HW_IO_INT_HBLANK);
    }
    for (int i=0; i<HW_SCREEN_HBLANK*HW_VPU_CPUCYCLE_RATIO; i++) {
      stepCpuCycle();
    }
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
  for (int i=0; i<HW_SCREEN_VBLANK * (HW_SCREEN_W + HW_SCREEN_HBLANK)*HW_VPU_CPUCYCLE_RATIO; i++) {
    stepCpuCycle();
  }
  io.clearIntStatus(HW_IO_INT_VBLANK);
}

void Board::stepCpuCycle()
{
  timer.stepCpuCycle();
  if (dma.isRunning()) {
    dma.stepCpuCycle();
  }
  else {
    cpu.stepCycle();
  }
}
