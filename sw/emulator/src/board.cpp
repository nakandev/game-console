#include <board.h>
#include <fmt/core.h>

Board::Board()
: masterClock(30*1024*1024),
  memory(),
  cpu(memory),
  intrrCtrl(memory, cpu),
  io(memory),
  vpu(memory),
  apu(memory),
  dma(memory, intrrCtrl),
  timer(memory, intrrCtrl),
  sram(memory, intrrCtrl),
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
  sram.init();
}

Board::~Board()
{
}

auto Board::reset() -> void
{
  memory.resetRamSections();
  cpu.init();
  vpu.init();
  apu.init();
  dma.init();
  timer.init();
  sram.init();
  pause = false;
}

auto Board::loadElf(const string& path) -> uint8_t
{
  memory.resetRamSections();
  auto ret = cpu.loadElf(path);
  if (ret) return ret;
  vpu.init();
  apu.init();
  dma.init();
  timer.init();
  sram.load(path);
  pause = false;
  return ret;
}

auto Board::stepCpuCycle() -> void
{
  timer.stepCycle();
  memory.processor->stepCycle();  // cpu or dma
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
  io.updateMusicFrameNumber();

  intrrCtrl.setIntStatus(HW_IO_INT_VBLANK);
  intrrCtrl.requestInt(HW_IO_INT_VBLANK);
  for (int i=0; i<HW_SCREEN_VBLANK * (HW_SCREEN_W + HW_SCREEN_HBLANK)*HW_VPU_CPUCYCLE_RATIO; i++) {
    stepCpuCycle();
  }
  intrrCtrl.clearIntStatus(HW_IO_INT_VBLANK);
}

auto Board::saveState(const string& path) -> void
{
}

auto Board::loadState(const string& path) -> void
{
}
