#include <stdint.h>
#include <memorymap.h>
#include <nkx/sw/nkx_sw.h>

namespace nkx {

/* video: tile mode */
void setPalette(uint8_t palNo, uint32_t offset, uint32_t* palData, uint32_t size)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  for (int i=0; i<size; i++) {
    // tileram.palette[palNo].color[i] = {.data=palData[i]};
    tileram.palette[palNo].color[i+offset] = {
      .a=(uint8_t)((palData[i]>>24)&0xffu),
      .b=(uint8_t)((palData[i]>>16)&0xffu),
      .g=(uint8_t)((palData[i]>> 8)&0xffu),
      .r=(uint8_t)((palData[i]>> 0)&0xffu),
    };
  }
}
void setTile(uint8_t tileNo, uint32_t offset, uint8_t* tileData, uint32_t size)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  for (int i=0; i<size; i++) {
    int x = i % 64 % 8;
    int y = i % 64 / 8;
    int t = i / 64;
    tileram.tile[tileNo][t+offset].data[y][x] = tileData[i];
  }
}
void setTilemap(uint8_t mapNo, uint32_t offset, uint8_t* mapData, uint32_t size)
{
}

/* video: text utility */
void loadAsciiFontPalette(uint8_t palNo, uint32_t* palData, uint32_t size)
{
  setPalette(palNo, 0, palData, size);
}
void loadAsciiFontTile(uint8_t tileNo, uint8_t* tileData, uint32_t size)
{
  setTile(tileNo, 0, tileData, size);
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

