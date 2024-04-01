#include <stdint.h>
#include <memorymap.h>
#include <nkx/mw/nkx_mw.h>

namespace nkx {

/* video: tile mode */
void setPalette(uint8_t palBank, uint8_t palNo, uint32_t offset, const uint32_t* palData, uint32_t size)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  for (int i=0; i<size; i++) {
    // tileram.palette[palNo].color[i] = {.data=palData[i]};
    tileram.palette[palBank].color[i+offset+palNo*16] = {
      .a=(uint8_t)((palData[i]>>24)&0xffu),
      .b=(uint8_t)((palData[i]>>16)&0xffu),
      .g=(uint8_t)((palData[i]>> 8)&0xffu),
      .r=(uint8_t)((palData[i]>> 0)&0xffu),
    };
  }
}
void setTile(uint8_t tileBank, uint32_t offset, const uint8_t* tileData, uint32_t size)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  for (int i=0; i<size; i++) {
    int x = i % 64 % 8;
    int y = i % 64 / 8;
    int t = i / 64;
    tileram.tile[tileBank][t+offset].data[y][x] = tileData[i];
  }
}
void setTilemap(uint8_t mapBank, uint32_t offset, const uint16_t* mapData, uint32_t size)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  for (int i=0; i<size; i++) {
    tileram.tilemap[mapBank].tileIdx[i].data = mapData[i];
  }
}

void setTilemap2D(
  uint8_t mapBank,
  uint32_t mapSizeW, uint32_t mapSizeH,
  uint32_t mapOffsetX, uint32_t mapOffsetY,
  const uint16_t* mapData,
  uint32_t dataSizeW, uint32_t dataSizeH,
  uint32_t dataRectX, uint32_t dataRectY, uint32_t dataRectW, uint32_t dataRectH
) {
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  for (int y=0; y<dataRectH; y++) {
    for (int x=0; x<dataRectW; x++) {
      int tileIdx = (x + mapOffsetX) % mapSizeW + (y + mapOffsetY) % mapSizeH * mapSizeW;
      int dataIdx = (x + dataRectX) % dataSizeW + (y + dataRectY) % dataSizeH * dataSizeW;
      tileram.tilemap[mapBank].tileIdx[tileIdx].data = mapData[dataIdx];
    }
  }
}

void setTilemap2DLambda(
  uint8_t mapBank,
  uint32_t mapSizeW, uint32_t mapSizeH,
  uint32_t mapOffsetX, uint32_t mapOffsetY,
  uint16_t (*mapData)(int32_t, int32_t),
  uint32_t dataSizeW, uint32_t dataSizeH,
  uint32_t dataRectX, uint32_t dataRectY, uint32_t dataRectW, uint32_t dataRectH
) {
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  for (int y=0; y<dataRectH; y++) {
    for (int x=0; x<dataRectW; x++) {
      int tileIdx = (x + mapOffsetX) % mapSizeW + (y + mapOffsetY) % mapSizeH * mapSizeW;
      int dataIdxX = (x + dataRectX) % dataSizeW;
      int dataIdxY = (y + dataRectY) % dataSizeH;
      tileram.tilemap[mapBank].tileIdx[tileIdx].data = mapData(dataIdxX, dataIdxY);
    }
  }
}

/* video: text utility */
void loadAsciiFontPalette(uint8_t palBank, uint8_t palNo, const uint32_t* palData, uint32_t size)
{
  setPalette(palBank, palNo, 0, palData, size);
}
void loadAsciiFontTile(uint8_t tileNo, const uint8_t* tileData, uint32_t size)
{
  setTile(tileNo, 0, tileData, size);
}
void setAsciiFontBG(uint8_t bgNo, uint8_t palBank, uint8_t palNo, uint8_t tileBank, uint8_t mapBank)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  tileram.bg[0].enable = true;
  tileram.bg[bgNo].paletteInfo.no = palBank;
  tileram.bg[bgNo].paletteInfo.no = palNo;
  tileram.bg[bgNo].tileBank = tileBank;
  tileram.bg[bgNo].tilemapBank = mapBank;
}

void putsBG(uint8_t bg, uint16_t x, uint16_t y, const char* str, uint8_t n)
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  HwBG& hwbg = tileram.bg[bg];
  int tilemapOffset = x + y * HW_TILEMAP_XTILE;
  for (int i=0; i<strlen(str, n); i++) {
    tileram.tilemap[hwbg.tilemapBank].tileIdx[tilemapOffset + i].data = str[i];
  }
}
}; /* namespace nkx*/

