#include <board.h>
#include <fmt/core.h>

Board::Board()
: masterClock(30*1024*1024),
  memory(),
  cpu(memory),
  intrrCtrl(memory, cpu),
  vpu(memory),
  apu(memory),
  dma(memory, intrrCtrl),
  timer(memory, intrrCtrl),
  io(memory),
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

auto Board::reset() -> void
{
  cpu.init();
  vpu.init();
  apu.init();
  dma.init();
  timer.init();
  pause = false;
}

auto Board::stepCpuCycle() -> void
{
  timer.stepCycle();
  if (memory.processor) {
    memory.processor->stepCycle();  // cpu or dma
  }
  else {
    cpu.stepCycle();
  }
}

auto Board::updateFrame() -> void
{
  if (pause) return;
  for (int y=0; y<HW_SCREEN_H; y++) {
    for (int i=0; i<HW_SCREEN_W*HW_VPU_CPUCYCLE_RATIO; i++) {
      stepCpuCycle();
    }
    vpu.drawLine(y);
    if (y != HW_SCREEN_H - 1) {
      intrrCtrl.setIntStatus(HW_IO_INT_HBLANK);
      intrrCtrl.requestInt(HW_IO_INT_HBLANK);
    }
    for (int i=0; i<HW_SCREEN_HBLANK*HW_VPU_CPUCYCLE_RATIO; i++) {
      stepCpuCycle();
    }
    if (y != HW_SCREEN_H - 1) {
      intrrCtrl.clearIntStatus(HW_IO_INT_HBLANK);
    }
    io.updateScanlineNumber(y);
  }
  apu.updateMusicBuffer();

  intrrCtrl.setIntStatus(HW_IO_INT_VBLANK);
  intrrCtrl.requestInt(HW_IO_INT_VBLANK);
  for (int i=0; i<HW_SCREEN_VBLANK * (HW_SCREEN_W + HW_SCREEN_HBLANK)*HW_VPU_CPUCYCLE_RATIO; i++) {
    stepCpuCycle();
  }
  intrrCtrl.clearIntStatus(HW_IO_INT_VBLANK);
}
