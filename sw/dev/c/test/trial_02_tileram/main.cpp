#include <nkx/hw/nkx_hw.h>
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
  nkx::hw.tile[0][0].data[0][6] = 2; nkx::hw.tile[0][0].data[2][6] = 7;
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

  /* main loop */
  while (1) {
    // nkx::hw.bg[0]->x = 0;
    volatile uint32_t pad = (*(uint32_t*)0x0300'0000);
    if (pad & 0x1000u) {
        nkx::hw.bg[0]->y -= 1;
    } else
    if (pad & 0x2000u) {
        nkx::hw.bg[0]->y += 1;
    }
    if (pad & 0x4000u) {
        nkx::hw.bg[0]->x -= 1;
    } else
    if (pad & 0x8000u) {
        nkx::hw.bg[0]->x += 1;
    }
  }

  return 0;
}
