#include <stdint.h>

namespace nkx {

enum {
  HWREG_IORAM_BASEADDR     = 0x0300'0000,
  HWREG_VRAM_BASEADDR      = 0x0400'0000,
  HWREG_TILERAM_BASEADDR   = 0x0600'0000,
  HWREG_PROGRAM_BASEADDR   = 0x0800'0000,

  HW_IO_PAD0 = HWREG_IORAM_BASEADDR + 0x0000,
  HW_IO_PAD1 = HWREG_IORAM_BASEADDR + 0x0010,

  HWCOLOR_BYTESIZE = 4,

  HWPALETTE_COLORNUM = 256,
  HWPALETTE_BYTESIZE = 1,
  HWREG_PALETTE_BASEADDR   = HWREG_TILERAM_BASEADDR + 0x0000'0000,
  HWREG_PALETTE0_ADDR    = HWREG_PALETTE_BASEADDR + 0x0000,
  HWREG_PALETTE1_ADDR    = HWREG_PALETTE_BASEADDR + 0x0400,
  HWREG_PALETTE2_ADDR    = HWREG_PALETTE_BASEADDR + 0x0800,
  HWREG_PALETTE3_ADDR    = HWREG_PALETTE_BASEADDR + 0x0C00,

  HWTILE_W = 8,
  HWTILE_H = 8,
  HWTILE_BYTESIZE = HWTILE_W * HWTILE_H * HWPALETTE_BYTESIZE,  // 64
  HWREG_TILE_BASEADDR      = HWREG_TILERAM_BASEADDR + 0x0010'0000,
  HWREG_TILE0_ADDR    = HWREG_TILE_BASEADDR + 0x00000000,
  HWREG_TILE1_ADDR    = HWREG_TILE_BASEADDR + 0x00010000,
  HWREG_TILE2_ADDR    = HWREG_TILE_BASEADDR + 0x00020000,
  HWREG_TILE3_ADDR    = HWREG_TILE_BASEADDR + 0x00030000,

  HWBG_TILENUM = 64 * 64,  // 4096
  HWREG_TILEMAP_BASEADDR   = HWREG_TILERAM_BASEADDR + 0x0020'0000,
  HWREG_TILEMAP0_ADDR      = HWREG_TILEMAP_BASEADDR + 0x0000,
  HWREG_TILEMAP1_ADDR      = HWREG_TILEMAP_BASEADDR + 0x1000,
  HWREG_TILEMAP2_ADDR      = HWREG_TILEMAP_BASEADDR + 0x2000,
  HWREG_TILEMAP3_ADDR      = HWREG_TILEMAP_BASEADDR + 0x3000,

  HW_SCREEN_W = 320,
  HW_SCREEN_H = 240,
  HW_SCREEN_XTILE = 40,
  HW_SCREEN_YTILE = 30,
  HW_TILEMAP_W = 512,
  HW_TILEMAP_H = 512,
  HW_TILEMAP_XTILE = 64,
  HW_TILEMAP_YTILE = 64,
  HWREG_BG_BASEADDR        = HWREG_TILERAM_BASEADDR + 0x0030'0000,
  HWREG_BG0_ADDR      = HWREG_BG_BASEADDR + 0x0000,

  HWREG_SPRITE_BASEADDR    = HWREG_TILERAM_BASEADDR + 0x0040'0000,
};


struct HwColor {
  uint8_t a;
  uint8_t b;
  uint8_t g;
  uint8_t r;

  HwColor();
  HwColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  HwColor(uint32_t data);
  ~HwColor();
  void set(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
  uint32_t data();
};

typedef union HwPalette {
  uint32_t data[HWPALETTE_COLORNUM];
  HwColor color[HWPALETTE_COLORNUM];
} HwPalette;

struct HwTile {
  uint8_t data[HWTILE_H][HWTILE_W];  // paletteIdx
};

struct HwTilemap {
  uint16_t tileIdx[HWBG_TILENUM];
};

struct HwBG {
  uint8_t paletteNo;
  uint8_t tileNo;
  uint8_t tilemapNo;
  uint16_t x;
  uint16_t y;
};

class Hw {
  public:
  HwPalette* palette[1];
  HwTile* tile[1];
  HwTilemap* tilemap[1];
  HwBG* bg[1];

  Hw();
  ~Hw();
};


#ifdef BUILD_FOR_SIMULATOR
extern uint8_t HwReg[0x4000'0000];
#else
extern uint8_t* HwReg;
#endif

extern volatile Hw hw;

}; /* namespace nkx*/

