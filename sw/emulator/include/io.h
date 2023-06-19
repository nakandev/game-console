#include <cstdint>
class Memory;
class Cpu;
class Ppu;

enum {
  HWREG_IO_BASEADDR = 0x0300'0000,
  HW_IO_PAD_ADDR = HWREG_IO_BASEADDR + 0x0000,
  HW_IO_EXTINT_ENABLE_ADDR = HWREG_IO_BASEADDR + 0x0100,
  HW_IO_EXTINT_STATUS_ADDR = HWREG_IO_BASEADDR + 0x0200,
  HW_IO_EXTINT_VECTOR_ADDR = HWREG_IO_BASEADDR + 0x1000,
};
enum {  // PAD_BUTTON_BIT
  PAD_A = 0,
  PAD_B,
  PAD_C,
  PAD_D,
  PAD_L,
  PAD_R,
  PAD_S,
  PAD_T,
  PAD_RESERVED0,
  PAD_RESERVED1,
  PAD_RESERVED2,
  PAD_RESERVED3,
  PAD_UP,
  PAD_DOWN,
  PAD_LEFT,
  PAD_RIGHT,
};
enum {  // INT_EXT_CAUSE_BIT / STATUS_BIT
  IO_EXTINT_HBLANK = 16,
  IO_EXTINT_VBLANK,
  IO_EXTINT_PAD,
  IO_EXTINT_DMA0,
  IO_EXTINT_DMA1,
  IO_EXTINT_DMA2,
  IO_EXTINT_DMA3,
  IO_EXTINT_SERIAL,
};

class IO {
private:
  Memory& memory;
  Cpu* cpu;
  Ppu* ppu;
public:
  IO(Memory& memory);
  ~IO();
  void setCpu(Cpu& cpu);
  void setPpu(Ppu& ppu);
  void pressPadButton(uint8_t button);
  void releasePadButton(uint8_t button);
  void requestExtInt(uint8_t intno);
  void setExtIntStatus(uint8_t intno);
  void clearExtIntStatus(uint8_t intno);
};
