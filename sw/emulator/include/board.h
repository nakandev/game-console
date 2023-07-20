#include <memorymap.h>
#include <memory.h>
#include <cpu.h>
#include <ppu.h>
#include <apu.h>
#include <io.h>

class Board {
public:
  Memory memory;
  IO io;
  Cpu cpu;
  Ppu ppu;
  Apu apu;
  Board();
  ~Board();
  void updateFrameUntilVblank();
  void updateFrameSinceVblank();
};
