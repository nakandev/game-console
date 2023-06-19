#include <memory.h>
#include <cpu.h>
#include <ppu.h>
#include <io.h>

class Board {
public:
  Memory memory;
  IO io;
  Cpu cpu;
  Ppu ppu;
  Board();
  ~Board();
  void updateFrameUntilVblank();
  void updateFrameSinceVblank();
};
