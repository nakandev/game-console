#include <nkx/hw/nkx_hw.h>
#include <nkx/sw/nkx_sw.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

const int16_t cosTable[256] = {
256,255,255,255,254,254,253,252,251,249,248,246,244,243,241,238,
236,234,231,228,225,222,219,216,212,209,205,201,197,193,189,185,
181,176,171,167,162,157,152,147,142,136,131,126,120,115,109,103,
97,92,86,80,74,68,62,56,49,43,37,31,25,18,12,6,
0,-6,-12,-18,-25,-31,-37,-43,-49,-56,-62,-68,-74,-80,-86,-92,
-97,-103,-109,-115,-120,-126,-131,-136,-142,-147,-152,-157,-162,-167,-171,-176,
-181,-185,-189,-193,-197,-201,-205,-209,-212,-216,-219,-222,-225,-228,-231,-234,
-236,-238,-241,-243,-244,-246,-248,-249,-251,-252,-253,-254,-254,-255,-255,-255,
-256,-255,-255,-255,-254,-254,-253,-252,-251,-249,-248,-246,-244,-243,-241,-238,
-236,-234,-231,-228,-225,-222,-219,-216,-212,-209,-205,-201,-197,-193,-189,-185,
-181,-176,-171,-167,-162,-157,-152,-147,-142,-136,-131,-126,-120,-115,-109,-103,
-97,-92,-86,-80,-74,-68,-62,-56,-49,-43,-37,-31,-25,-18,-12,-6,
0,6,12,18,25,31,37,43,49,56,62,68,74,80,86,92,
97,103,109,115,120,126,131,136,142,147,152,157,162,167,171,176,
181,185,189,193,197,201,205,209,212,216,219,222,225,228,231,234,
236,238,241,243,244,246,248,249,251,252,253,254,254,255,255,255
};
namespace nkx {
__attribute__((noinline))
int16_t cos(uint8_t val) {
  return cosTable[val];
}
__attribute__((noinline))
int16_t sin(uint8_t val) {
  if (64 - val < 0) return cosTable[(256 + 64 - val)];
  else              return cosTable[(64 - val)];
}
};

extern "C" {
uint8_t rad = 0;
int16_t scale = 0;
void int_handler()
{
  volatile uint32_t pad = (*(uint32_t*)0x0300'0000);
  if (pad & 0x1000u) {
    nkx::hw.sp[0][0].sprite[0].y -= 1;
    nkx::hw.sp[0][0].sprite[1].affineInv.y -= 1;
  } else
  if (pad & 0x2000u) {
    nkx::hw.sp[0][0].sprite[0].y += 1;
    nkx::hw.sp[0][0].sprite[1].affineInv.y += 1;
  }
  if (pad & 0x4000u) {
    nkx::hw.sp[0][0].sprite[0].x -= 1;
    nkx::hw.sp[0][0].sprite[1].affineInv.x -= 1;
  } else
  if (pad & 0x8000u) {
    nkx::hw.sp[0][0].sprite[0].x += 1;
    nkx::hw.sp[0][0].sprite[1].affineInv.x += 1;
  }

  if (pad & 0x0001u) {
    rad += 1;
    scale += 2;
  }
  if (pad & 0x0002u) {
    rad -= 1;
    scale -= 2;
  }

  if (pad & 0x0004u) {
    nkx::hw.sp[0][0].sprite[0].x = HW_SCREEN_W / 2 - 4;
    nkx::hw.sp[0][0].sprite[0].y = HW_SCREEN_H / 2 - 4;
    nkx::hw.sp[0][0].sprite[1].affineInv.x = 32;
    nkx::hw.sp[0][0].sprite[1].affineInv.y = 32;
  }
  if (pad & 0x0008u) {
    rad = 0;
    scale = 0x100;
  }

  nkx::hw.sp[0][0].sprite[1].affineInv.a =  nkx::cos(rad);
  nkx::hw.sp[0][0].sprite[1].affineInv.b =  nkx::sin(rad);
  nkx::hw.sp[0][0].sprite[1].affineInv.c = -nkx::sin(rad);
  nkx::hw.sp[0][0].sprite[1].affineInv.d =  nkx::cos(rad);
  // nkx::hw.sp[0][0].sprite[1].affineInv.a = scale;
  // nkx::hw.sp[0][0].sprite[1].affineInv.b = 0x0000;
  // nkx::hw.sp[0][0].sprite[1].affineInv.c = 0x0000;
  // nkx::hw.sp[0][0].sprite[1].affineInv.d = scale;
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
  nkx::hw.bg[0]->flag.affineEnable = true;
  nkx::hw.bg[0]->affineInv.a = -0x0100;
  nkx::hw.bg[0]->affineInv.b =  0x0000;
  nkx::hw.bg[0]->affineInv.c =  0x0000;
  nkx::hw.bg[0]->affineInv.d =  0x0080;
  nkx::hw.bg[0]->affineInv.x = HW_SCREEN_W / 2;
  nkx::hw.bg[0]->affineInv.y = 0;
  // nkx::hw.bg[1]->flag.affineEnable = true;
  // nkx::hw.bg[2]->flag.affineEnable = true;
  // nkx::hw.bg[3]->flag.affineEnable = true;
  nkx::hw.bg[0]->flag.layer = 2;
  nkx::hw.bg[1]->flag.layer = 1;
  nkx::hw.bg[2]->flag.layer = 0;
  nkx::hw.bg[3]->flag.layer = 0;
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
  nkx::hw.sp[0][0].sprite[0].flag.enable = true;
  nkx::hw.sp[0][0].sprite[0].paletteNo = 0;
  nkx::hw.sp[0][0].sprite[0].tileNo = sptileNo;
  nkx::hw.sp[0][0].sprite[0].tileIdx = 0;
  nkx::hw.sp[0][0].sprite[0].tileSize = HW_SPRITE_TILESIZE_8x8;
  nkx::hw.sp[0][0].sprite[0].x = HW_SCREEN_W / 2 - 4;
  nkx::hw.sp[0][0].sprite[0].y = HW_SCREEN_H / 2 - 4;

  nkx::hw.sp[0][0].sprite[1].flag.enable = true;
  nkx::hw.sp[0][0].sprite[1].paletteNo = 0;
  nkx::hw.sp[0][0].sprite[1].tileNo = sptileNo;
  nkx::hw.sp[0][0].sprite[1].tileIdx = 0;
  nkx::hw.sp[0][0].sprite[1].tileSize = HW_SPRITE_TILESIZE_64x64;
  nkx::hw.sp[0][0].sprite[1].x = HW_SCREEN_W / 2 - 32;
  nkx::hw.sp[0][0].sprite[1].y = HW_SCREEN_H / 2 - 32;
  nkx::hw.sp[0][0].sprite[1].flag.affineEnable = true;
  nkx::hw.sp[0][0].sprite[1].affineInv.a =  nkx::cos(31);
  nkx::hw.sp[0][0].sprite[1].affineInv.b =  nkx::sin(31);
  nkx::hw.sp[0][0].sprite[1].affineInv.c = -nkx::sin(31);
  nkx::hw.sp[0][0].sprite[1].affineInv.d =  nkx::cos(31);
  nkx::hw.sp[0][0].sprite[1].affineInv.x = 32;
  nkx::hw.sp[0][0].sprite[1].affineInv.y = 32;

  /* main loop */
  while (1) {
  }

  return 0;
}
