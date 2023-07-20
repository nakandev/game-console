#include <nkx/hw/nkx_hw.h>
#include <nkx/sw/nkx_sw.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
void int_handler()
{
}
};

int main()
{
  // HwTileRam *_hwTileRam = (HwTileRam*)HWREG_TILERAM_BASEADDR;
  // HwTileRam& hwTileRam = *_hwTileRam;
  HwTileRam& hwTileRam = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  nkx::srand(0x1234);
  asm("nop");
  asm("nop");
  /* palette settings */
  hwTileRam.palette[0].color[ 0] = {.data=0x00000000};
  hwTileRam.palette[0].color[ 1] = {.data=0x000000ff};
  hwTileRam.palette[0].color[ 2] = {.data=0xffffffff};
  hwTileRam.palette[0].color[ 3] = {.data=0xff0000ff};
  hwTileRam.palette[0].color[ 4] = {.data=0xffff00ff};
  hwTileRam.palette[0].color[ 5] = {.data=0x00ff00ff};
  hwTileRam.palette[0].color[ 6] = {.data=0x00ffffff};
  hwTileRam.palette[0].color[ 7] = {.data=0x0000ffff};
  hwTileRam.palette[0].color[ 8] = {.data=0xff00ffff};
  hwTileRam.palette[0].color[ 9] = {.data=0x7f0000ff};
  hwTileRam.palette[0].color[10] = {.data=0x7f0000ff};
  hwTileRam.palette[0].color[11] = {.data=0x7f7f00ff};
  hwTileRam.palette[0].color[12] = {.data=0x007f00ff};
  hwTileRam.palette[0].color[13] = {.data=0x007f7fff};
  hwTileRam.palette[0].color[14] = {.data=0x00007fff};
  hwTileRam.palette[0].color[15] = {.data=0x7f007fff};
  asm("nop");
  asm("nop");
  /* tile settings */
  hwTileRam.tile[0][0].data[4][4] = 2;

  const int tileno = 1;
  for (int x=0; x<8; x++) {
    hwTileRam.tile[0][tileno].data[0][x] = 3;
    hwTileRam.tile[0][tileno].data[7][x] = 3;
  }
  for (int y=0; y<8; y++) {
    hwTileRam.tile[0][tileno].data[y][0] = 5;
    hwTileRam.tile[0][tileno].data[y][7] = 5;
  }
  /* tilemap settings */
  hwTileRam.tilemap[0].tileIdx[1] = tileno;
  hwTileRam.tilemap[0].tileIdx[3] = tileno;
  hwTileRam.tilemap[1].tileIdx[1] = tileno;
  hwTileRam.tilemap[1].tileIdx[3] = tileno;
  /* BG settings */
  hwTileRam.bg[0].paletteNo = 0;
  hwTileRam.bg[0].tileNo = 0;
  hwTileRam.bg[0].tilemapNo = 1;
  hwTileRam.bg[0].x = 0;
  hwTileRam.bg[0].y = 32;
  hwTileRam.bg[0].flag.affineEnable = true;
  hwTileRam.bg[0].affineInv.a = -0x0100;
  hwTileRam.bg[0].affineInv.b =  0x0000;
  hwTileRam.bg[0].affineInv.c =  0x0000;
  hwTileRam.bg[0].affineInv.d =  0x0080;
  hwTileRam.bg[0].affineInv.x = HW_SCREEN_W / 2;
  hwTileRam.bg[0].affineInv.y = 0;
  hwTileRam.bg[0].flag.layer = 2;
  hwTileRam.bg[1].flag.layer = 1;
  hwTileRam.bg[2].flag.layer = 0;
  hwTileRam.bg[3].flag.layer = 0;
  /* SP tile settings */
  int sptileNo = 15;
  const int tidxTbl[] = {0, 3, 7, 12, 15, 56, 63};
  for (int tTi=0; tTi<sizeof(tidxTbl)/sizeof(tidxTbl[0]); tTi++) {
    int tidx = tidxTbl[tTi];
    for (int i=0; i<8; i++) {
      hwTileRam.tile[sptileNo][tidx].data[0][i] = 3;
      hwTileRam.tile[sptileNo][tidx].data[7][i] = 3;
      hwTileRam.tile[sptileNo][tidx].data[i][0] = 5;
      hwTileRam.tile[sptileNo][tidx].data[i][7] = 5;
    }
  }
  /* SP sprite settings */
  hwTileRam.sp[0].sprite[0].flag.enable = true;
  hwTileRam.sp[0].sprite[0].paletteNo = 0;
  hwTileRam.sp[0].sprite[0].tileNo = sptileNo;
  hwTileRam.sp[0].sprite[0].tileIdx = 0;
  hwTileRam.sp[0].sprite[0].tileSize = HW_SPRITE_TILESIZE_8x8;
  hwTileRam.sp[0].sprite[0].x = HW_SCREEN_W / 2 - 4;
  hwTileRam.sp[0].sprite[0].y = HW_SCREEN_H / 2 - 4;

  /* main loop */
  while (1) {
  }

  return 0;
}
