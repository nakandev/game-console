#include <stdint.h>
#include <memorymap.h>

namespace nkx {

static const int32_t _randvalue1 = 1234567;
static const int32_t _randvalue2 = 1357;
static int32_t _randvalue = 0;

__attribute__((noinline))
void srand(int32_t seed)
{
  _randvalue = seed;
}

__attribute__((noinline))
int32_t rand()
{
  _randvalue *= _randvalue1;
  _randvalue += _randvalue2;
  return _randvalue;
}

uint8_t strlen(const char* str, uint8_t n)
{
  int i = 0;
  for (i=0; i<n; i++) {
    if (str[i] == '\0') return i;
  }
  return i;
}

void loadAsciiFontPalette(uint8_t palNo, uint32_t* palData, uint32_t size)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  for (int i=0; i<size; i++) {
    tileram.palette[palNo].color[i] = {.data=palData[i]};
  }
}
void loadAsciiFontTile(uint8_t tileNo, uint8_t* tileData, uint32_t size)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  for (int i=0; i<size; i++) {
    int x = i % 64 % 8;
    int y = i % 64 / 8;
    int t = i / 64;
    tileram.tile[tileNo][t].data[y][x] = tileData[i];
  }
}
void setAsciiFontBG(uint8_t bgNo, uint8_t palNo, uint8_t tileNo, uint8_t tilemapNo)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  tileram.bg[0].flag.enable = true;
  tileram.bg[bgNo].paletteNo = palNo;
  tileram.bg[bgNo].tileNo = tileNo;
  tileram.bg[bgNo].tilemapNo = tilemapNo;
}

void putsBG(uint8_t bg, uint16_t x, uint16_t y, const char* str, uint8_t n)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  HwBG& hwbg = tileram.bg[bg];
  int tilemapOffset = x + y * HW_TILEMAP_XTILE;
  for (int i=0; i<strlen(str, n); i++) {
    tileram.tilemap[hwbg.tilemapNo].tileIdx[tilemapOffset + i] = str[i];
  }
}
}; /* namespace nkx*/

