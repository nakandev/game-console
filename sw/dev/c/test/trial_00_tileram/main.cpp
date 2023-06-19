#include <nkx/hw/nkx_hw.h>
#include <nkx/util/nkx_image.h>
#include <cstdio>
#include <cstring>

int main()
{
  memset(nkx::HwReg, sizeof(nkx::HwReg), 0);
  /* palette settings */
  nkx::hw.palette[0][0].color[0].set(0, 0, 0, 0);
  nkx::hw.palette[0][0].color[1].set(0, 0, 0, 255);
  nkx::hw.palette[0][0].color[2].set(255, 255, 255, 255);
  nkx::hw.palette[0][0].color[3].set(255,   0,   0, 255);
  nkx::hw.palette[0][0].color[4].set(255, 255,   0, 255);
  nkx::hw.palette[0][0].color[5].set(  0, 255,   0, 255);
  nkx::hw.palette[0][0].color[6].set(  0, 255, 255, 255);
  nkx::hw.palette[0][0].color[7].set(  0,   0, 255, 255);
  nkx::hw.palette[0][0].color[8].set(255,   0, 255, 255);
  nkx::hw.palette[0][0].color[9].set(127,   0,   0, 255);
  nkx::hw.palette[0][0].color[10].set(127,   0,   0, 255);
  nkx::hw.palette[0][0].color[11].set(127, 127,   0, 255);
  nkx::hw.palette[0][0].color[12].set(  0, 127,   0, 255);
  nkx::hw.palette[0][0].color[13].set(  0, 127, 127, 255);
  nkx::hw.palette[0][0].color[14].set(  0,   0, 127, 255);
  nkx::hw.palette[0][0].color[15].set(127,   0, 127, 255);
  /* tile settings */
  nkx::hw.tile[0][0].data[0][0] = 1; nkx::hw.tile[0][0].data[2][0] = 0;
  nkx::hw.tile[0][0].data[0][1] = 3; nkx::hw.tile[0][0].data[2][1] = 2;
  nkx::hw.tile[0][0].data[0][2] = 4; nkx::hw.tile[0][0].data[2][2] = 3;
  nkx::hw.tile[0][0].data[0][3] = 5; nkx::hw.tile[0][0].data[2][3] = 4;
  nkx::hw.tile[0][0].data[0][4] = 6; nkx::hw.tile[0][0].data[2][4] = 5;
  nkx::hw.tile[0][0].data[0][5] = 7; nkx::hw.tile[0][0].data[2][5] = 6;
  nkx::hw.tile[0][0].data[0][6] = 8; nkx::hw.tile[0][0].data[2][6] = 7;
  nkx::hw.tile[0][0].data[0][7] = 0; nkx::hw.tile[0][0].data[2][7] = 8;
  nkx::hw.tile[0][1].data[0][0] = 8;
  nkx::hw.tile[0][1].data[0][1] = 9;
  nkx::hw.tile[0][1].data[0][2] = 10;
  nkx::hw.tile[0][1].data[0][3] = 11;
  nkx::hw.tile[0][1].data[0][4] = 12;
  nkx::hw.tile[0][1].data[0][5] = 13;
  nkx::hw.tile[0][1].data[0][6] = 14;
  nkx::hw.tile[0][1].data[0][7] = 15;
  /* tilemap settings */
  nkx::hw.tilemap[0][0].tileIdx[1] = 1;
  nkx::hw.tilemap[0][0].tileIdx[3] = 1;
  /* BG settings */
  nkx::hw.bg[0]->paletteNo = 0;
  nkx::hw.bg[0]->tileNo = 0;
  nkx::hw.bg[0]->tilemapNo = 0;
  nkx::hw.bg[0]->x = 0;
  nkx::hw.bg[0]->y = 20;

  /* output image */
  ImageData* imageData = newImageData(320, 240, 4);
  uint32_t* pix = (uint32_t*)(imageData->data);
  for (int y=0; y<240; y++) {
    for (int x=0; x<320; x++) {
      uint8_t tilemapNo = nkx::hw.bg[0]->tilemapNo;
      uint8_t tileNo = nkx::hw.bg[0]->tileNo;
      uint8_t paletteNo = nkx::hw.bg[0]->paletteNo;

      int x0 = ((uint32_t)(x - nkx::hw.bg[0]->x)) % nkx::HW_TILEMAP_W;
      int y0 = ((uint32_t)(y - nkx::hw.bg[0]->y)) % nkx::HW_TILEMAP_H;
      uint16_t tileIdx = nkx::hw.tilemap[tilemapNo]->tileIdx[(x0/8) + (y0/8)*nkx::HW_TILEMAP_XTILE];
      uint8_t paletteIdx = nkx::hw.tile[tileNo][tileIdx].data[y0%8][x0%8];
      nkx::HwColor* color = &nkx::hw.palette[paletteNo]->color[paletteIdx];

      pix[x + imageData->width*y] = color->data();
    }
  }
  pngFileEncodeWrite(imageData, "output.png");
  freeImageData(imageData);

  return 0;
}

