#include <ppu.h>
#include <memory.h>
#include <io.h>
#include <fmt/core.h>

enum {
  HWPALETTE_COLORNUM = 256,
  HWTILE_W = 8,
  HWTILE_H = 8,
  HWBG_TILENUM = 64 * 64,  // 4096
  HW_TILEMAP_W = 512,
  HW_TILEMAP_H = 512,
  HW_TILEMAP_XTILE = 64,
  HW_TILEMAP_YTILE = 64,
  // HWREG_VRAM_BASEADDR      = 0x0400'0000,
  // HWREG_TILERAM_BASEADDR   = 0x0600'0000,
  HWREG_VRAM_BASEADDR      = 0x0,
  HWREG_TILERAM_BASEADDR   = 0x0,
  HWREG_PALETTE_BASEADDR   = HWREG_TILERAM_BASEADDR + 0x0000'0000,
  HWREG_PALETTE0_ADDR    = HWREG_PALETTE_BASEADDR + 0x0000,
  HWREG_PALETTE1_ADDR    = HWREG_PALETTE_BASEADDR + 0x0400,
  HWREG_TILE_BASEADDR      = HWREG_TILERAM_BASEADDR + 0x0010'0000,
  HWREG_TILE0_ADDR    = HWREG_TILE_BASEADDR + 0x00000000,
  HWREG_TILE1_ADDR    = HWREG_TILE_BASEADDR + 0x00010000,
  HWREG_TILEMAP_BASEADDR   = HWREG_TILERAM_BASEADDR + 0x0020'0000,
  HWREG_TILEMAP0_ADDR      = HWREG_TILEMAP_BASEADDR + 0x0000,
  HWREG_TILEMAP1_ADDR      = HWREG_TILEMAP_BASEADDR + 0x1000,
  HWREG_BG_BASEADDR        = HWREG_TILERAM_BASEADDR + 0x0030'0000,
  HWREG_BG0_ADDR      = HWREG_BG_BASEADDR + 0x0000,
  HWREG_BG1_ADDR      = HWREG_BG_BASEADDR + 0x0100,
  HWREG_BG2_ADDR      = HWREG_BG_BASEADDR + 0x0200,
  HWREG_BG3_ADDR      = HWREG_BG_BASEADDR + 0x0300,
};
typedef union HwColor {
  uint32_t data;
  struct {
    uint8_t a;
    uint8_t b;
    uint8_t g;
    uint8_t r;
  };
  void merge(const HwColor& dst){
    uint8_t a = dst.a;
    this->r = (uint8_t)(((uint16_t)this->r * (255 - a) + (uint16_t)dst.r * a) / 256);
    this->g = (uint8_t)(((uint16_t)this->g * (255 - a) + (uint16_t)dst.g * a) / 256);
    this->b = (uint8_t)(((uint16_t)this->b * (255 - a) + (uint16_t)dst.b * a) / 256);
  }
} HwColor;
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
  uint8_t __padding;
  uint16_t x;
  uint16_t y;
};

Ppu::Ppu(Memory& memory)
  : _currentLineNo(),
    lineBufferBg0(),
    lineBufferBg1(),
    lineBufferBg2(),
    lineBufferBg3(),
    lineBufferSp(),
    screenBuffer(),
    memory(memory),
    scanline()
{
  lineBufferBg0.resize(320);
  lineBufferBg1.resize(320);
  lineBufferBg2.resize(320);
  lineBufferBg3.resize(320);
  lineBufferSp.resize(320);
  screenBuffer.resize(320 * 240);
}

Ppu::~Ppu()
{
  lineBufferBg0.clear();
  lineBufferBg1.clear();
  lineBufferBg2.clear();
  lineBufferBg3.clear();
  lineBufferSp.clear();
  screenBuffer.clear();
}

int Ppu::currentLineNo()
{
  return _currentLineNo;
}

void Ppu::drawLine(int y)
{
  // const uint8_t* vram = memory.section("vram").buffer();
  const uint8_t* tileram = memory.section("tile").buffer();
  HwPalette* palette[1] = {(HwPalette*)(tileram + HWREG_PALETTE0_ADDR)};
  HwTile* tile[] = {(HwTile*)(tileram + HWREG_TILE0_ADDR)};
  HwTilemap* tilemap[1] = {(HwTilemap*)(tileram + HWREG_TILEMAP0_ADDR)};
  HwBG* bg[4] = {
    (HwBG*)(tileram + HWREG_BG0_ADDR),
    (HwBG*)(tileram + HWREG_BG1_ADDR),
    (HwBG*)(tileram + HWREG_BG2_ADDR),
    (HwBG*)(tileram + HWREG_BG3_ADDR),
  };
  uint8_t tilemapNo = bg[0]->tilemapNo;
  uint8_t tileNo = bg[0]->tileNo;
  uint8_t paletteNo = bg[0]->paletteNo;
  int yy[4];
  for (int i=0; i<4; i++) {
    yy[i] = ((uint32_t)(y - bg[i]->y)) % HW_TILEMAP_H;
  }
  for (int x=0; x<320; x++) {
    HwColor finalColor;
    HwColor bgColor[4];
    for (int i=0; i<4; i++) {
      int x0 = ((uint32_t)(x - bg[0]->x)) % HW_TILEMAP_W;
      uint16_t tileIdx = tilemap[tilemapNo]->tileIdx[(x0/8) + (yy[i]/8)*HW_TILEMAP_XTILE];
      uint8_t paletteIdx = tile[tileNo][tileIdx].data[yy[i]%8][x0%8];
      bgColor[i] = palette[paletteNo]->color[paletteIdx];
    }
    finalColor.data = bgColor[0].data;
    for (int i=1; i<4; i++) {
      finalColor.merge(bgColor[i]);
    }
    finalColor.a = 255;
    screenBuffer[x + y*320] = finalColor.data;
  }
}

void Ppu::drawAllLine()
{
  for (int y=0; y<240; y++) {
    drawLine(y);
  }
}

void Ppu::copyScreenBuffer(uint32_t* buffer)
{
  const uint8_t* tileram = memory.section("tile").buffer();
  HwBG* bg[1] = {(HwBG*)(tileram + HWREG_BG0_ADDR)};
  for (int i=0; i<320 * 240; i++) {
    buffer[i] = screenBuffer[i];
  }
}

void Ppu::printTile()
{
  const uint8_t* tileram = memory.section("tile").buffer();
  HwPalette* palette[1] = {(HwPalette*)(tileram + HWREG_PALETTE0_ADDR)};
  HwTile* tile[] = {(HwTile*)(tileram + HWREG_TILE0_ADDR)};
  HwTilemap* tilemap[1] = {(HwTilemap*)(tileram + HWREG_TILEMAP0_ADDR)};
  HwBG* bg[4] = {
    (HwBG*)(tileram + HWREG_BG0_ADDR),
    (HwBG*)(tileram + HWREG_BG1_ADDR),
    (HwBG*)(tileram + HWREG_BG2_ADDR),
    (HwBG*)(tileram + HWREG_BG3_ADDR),
  };
  uint8_t tilemapNo = 0;
  uint8_t tileNo = 0;
  uint8_t paletteNo = 0;
  fmt::print("tileram={}, tile[0]={}, pal[0]={}\n", (void*)(tileram), (void*)(tile[0]), (void*)(palette[0]));
  fmt::print("sizeof_HwColor={}, sizeof_HwPalette={}\n", sizeof(HwColor), sizeof(HwPalette));
  fmt::print("&tile.data={}\n", (void*)&(tile[0][0].data[0%8][0%8]));
  for (int y=0; y< 8; y++) {
    int y0 = y + 0;
    for (int x=0; x<8; x++) {
      int x0 = x + 0;
      uint16_t tileIdx = tilemap[tilemapNo]->tileIdx[(x0/8) + (y0/8)*HW_TILEMAP_XTILE];
      uint8_t paletteIdx = tile[tileNo][tileIdx].data[y0%8][x0%8];
      HwColor* color = &palette[paletteNo]->color[paletteIdx];
      fmt::print(" [{:2d}][{:2d}]{:08x}", tileIdx, paletteIdx, color->data);
      fmt::print(" {}", static_cast<void*>(&tile[tileNo][tileIdx].data[y0%8][x0%8]));
    }
    fmt::print("\n");
  }
}
