#include <nkx/hw/nkx_hw.h>
#include <nkx/sw/nkx_sw.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
int32_t time = 0;
uint8_t dir[200] = {0};
void int_handler()
{
  volatile uint32_t pad = (*(uint32_t*)0x0300'0000);
  if (pad & 0x1000u) {
      nkx::hw.sp[0][0].sprite[0].y -= 1;
  } else
  if (pad & 0x2000u) {
      nkx::hw.sp[0][0].sprite[0].y += 1;
  }
  if (pad & 0x4000u) {
      nkx::hw.sp[0][0].sprite[0].x -= 1;
  } else
  if (pad & 0x8000u) {
      nkx::hw.sp[0][0].sprite[0].x += 1;
  }

  if (pad & 0x0001u) {
      nkx::hw.sp[0][0].sprite[0].affineInv.x += 1;
  }
  if (pad & 0x0002u) {
      nkx::hw.sp[0][0].sprite[0].affineInv.y += 1;
  }
  if (pad & 0x0004u) {
      nkx::hw.sp[0][0].sprite[0].affineInv.y -= 1;
  }
  if (pad & 0x0008u) {
      nkx::hw.sp[0][0].sprite[0].affineInv.x -= 1;
  }

  if (time == 0) {
    for (int i=0; i<sizeof(dir); i++)
      dir[i] = nkx::rand() % 5;
  }
  time++;
  if (time & 0x20) {
    time = 0;
  }
  for (int i=0; i<sizeof(dir); i++) {
    if (dir[i] == 0) {
      if (nkx::hw.sp[0][0].sprite[i+1].y > 0)
        nkx::hw.sp[0][0].sprite[i+1].y -= 1;
    } else
    if (dir[i] == 1) {
      if (nkx::hw.sp[0][0].sprite[i+1].y < HW_SCREEN_H - 16)
        nkx::hw.sp[0][0].sprite[i+1].y += 1;
    } else
    if (dir[i] == 2) {
      if (nkx::hw.sp[0][0].sprite[i+1].x > 0)
        nkx::hw.sp[0][0].sprite[i+1].x -= 1;
    } else
    if (dir[i] == 3) {
      if (nkx::hw.sp[0][0].sprite[i+1].x < HW_SCREEN_W - 16)
        nkx::hw.sp[0][0].sprite[i+1].x += 1;
    } else
    {
    }
  }

}
};

int main()
{
  nkx::srand(0x1234);
  asm("nop");
  asm("nop");
  /* palette settings */
  nkx::hw.palette[0][0].color[ 0] = {.data=0x00000000};
  nkx::hw.palette[0][0].color[ 1] = {.data=0x000000ff};
  nkx::hw.palette[0][0].color[ 2] = {.data=0xffffffff};
  nkx::hw.palette[0][0].color[ 3] = {.data=0xff0000ff};
  nkx::hw.palette[0][0].color[ 4] = {.data=0xffff00ff};
  nkx::hw.palette[0][0].color[ 5] = {.data=0x00ff00ff};
  nkx::hw.palette[0][0].color[ 6] = {.data=0x00ffffff};
  nkx::hw.palette[0][0].color[ 7] = {.data=0x0000ffff};
  nkx::hw.palette[0][0].color[ 8] = {.data=0xff00ffff};
  nkx::hw.palette[0][0].color[ 9] = {.data=0x7f0000ff};
  nkx::hw.palette[0][0].color[10] = {.data=0x7f0000ff};
  nkx::hw.palette[0][0].color[11] = {.data=0x7f7f00ff};
  nkx::hw.palette[0][0].color[12] = {.data=0x007f00ff};
  nkx::hw.palette[0][0].color[13] = {.data=0x007f7fff};
  nkx::hw.palette[0][0].color[14] = {.data=0x00007fff};
  nkx::hw.palette[0][0].color[15] = {.data=0x7f007fff};
  asm("nop");
  asm("nop");
  /* tile settings */
  nkx::hw.tile[0][0].data[4][4] = 2;

  const int tileno = 1;
  for (int x=0; x<8; x++) {
    nkx::hw.tile[0][tileno].data[0][x] = 3;
    nkx::hw.tile[0][tileno].data[7][x] = 3;
  }
  for (int y=0; y<8; y++) {
    nkx::hw.tile[0][tileno].data[y][0] = 5;
    nkx::hw.tile[0][tileno].data[y][7] = 5;
  }
  /* tilemap settings */
  nkx::hw.tilemap[1][0].tileIdx[1] = tileno;
  nkx::hw.tilemap[1][0].tileIdx[3] = tileno;
  /* BG settings */
  nkx::hw.bg[0]->paletteNo = 0;
  nkx::hw.bg[0]->tileNo = 0;
  nkx::hw.bg[0]->tilemapNo = 1;
  nkx::hw.bg[0]->x = 0;
  nkx::hw.bg[0]->y = 0;
  /* SP tile settings */
  int sptileNo = 15;
  const int tidxTbl[] = {0, 3, 7, 12, 15, 56, 63};
  for (int tTi=0; tTi<sizeof(tidxTbl)/sizeof(tidxTbl[0]); tTi++) {
    int tidx = tidxTbl[tTi];
    for (int i=0; i<8; i++) {
      nkx::hw.tile[sptileNo][tidx].data[0][i] = 3;
      nkx::hw.tile[sptileNo][tidx].data[7][i] = 3;
      nkx::hw.tile[sptileNo][tidx].data[i][0] = 5;
      nkx::hw.tile[sptileNo][tidx].data[i][7] = 5;
    }
  }
  /* SP sprite settings */
  for (int i=0; i<sizeof(dir); i++) {
    nkx::hw.sp[0][0].sprite[i+1].flag.enable = true;
    nkx::hw.sp[0][0].sprite[i+1].paletteNo = 0;
    nkx::hw.sp[0][0].sprite[i+1].tileNo = sptileNo;
    nkx::hw.sp[0][0].sprite[i+1].tileIdx = 0;
    nkx::hw.sp[0][0].sprite[i+1].tileSize = HW_SPRITE_TILESIZE_16x16;
    nkx::hw.sp[0][0].sprite[i+1].x = ((uint32_t)nkx::rand() % 20) * 16;
    nkx::hw.sp[0][0].sprite[i+1].y = ((uint32_t)nkx::rand() % 15) * 16;
  }
  nkx::hw.sp[0][0].sprite[0].flag.enable = true;
  nkx::hw.sp[0][0].sprite[0].paletteNo = 0;
  nkx::hw.sp[0][0].sprite[0].tileNo = sptileNo;
  nkx::hw.sp[0][0].sprite[0].tileIdx = 0;
  nkx::hw.sp[0][0].sprite[0].tileSize = HW_SPRITE_TILESIZE_64x64;
  nkx::hw.sp[0][0].sprite[0].x = 0;
  nkx::hw.sp[0][0].sprite[0].y = 0;

  /* main loop */
  while (1) {
  }

  return 0;
}
