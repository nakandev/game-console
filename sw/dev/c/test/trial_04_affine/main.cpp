#include <nkx/hw/nkx_hw.h>
#include <nkx/mw/nkx_mw.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern uint8_t _binary_tile_gif_pal_start;
extern uint8_t _binary_tile_gif_pal_size;
extern uint8_t _binary_tile_gif_tile_start;
extern uint8_t _binary_tile_gif_tile_size;

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
void int_handler()
{
  HwIoRam& ioram = *(HwIoRam*)HWREG_IORAM_BASEADDR;
  if (!(ioram.intr.status.bits & (1<<HW_IO_INT_VBLANK))) return;

  HwTileRam& hwTileRam = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  // rotate
  hwTileRam.sp[0].sprite[0].affineInv.a =  nkx::cos(rad);
  hwTileRam.sp[0].sprite[0].affineInv.b = -nkx::sin(rad);
  hwTileRam.sp[0].sprite[0].affineInv.c =  nkx::sin(rad);
  hwTileRam.sp[0].sprite[0].affineInv.d =  nkx::cos(rad);
  // scale
  hwTileRam.sp[0].sprite[1].affineInv.a = (nkx::cos(rad)/2 + 0x140) * 1;
  hwTileRam.sp[0].sprite[1].affineInv.b = 0x0000;
  hwTileRam.sp[0].sprite[1].affineInv.c = 0x0000;
  hwTileRam.sp[0].sprite[1].affineInv.d = (nkx::sin(rad)/2 + 0x140) * 1;
  // sheer
  hwTileRam.sp[0].sprite[2].affineInv.a = 0x0100;
  hwTileRam.sp[0].sprite[2].affineInv.b = nkx::sin(rad) / 2;
  hwTileRam.sp[0].sprite[2].affineInv.c = 0x0000;
  hwTileRam.sp[0].sprite[2].affineInv.d = 0x0100;

  rad = (rad + 1) & 0xffu;
}
};

int main()
{
  HwIoRam& ioram = *(HwIoRam*)HWREG_IORAM_BASEADDR;
  ioram.intr.enable.bits = (1<<HW_IO_INT_HBLANK) | (1<<HW_IO_INT_VBLANK);

  HwTileRam& hwTileRam = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  nkx::srand(0x1234);
  asm("nop");
  asm("nop");
  /* palette settings */
  nkx::setPalette(0, 1, 0,
    (uint32_t*)&_binary_tile_gif_pal_start,
    (size_t)   &_binary_tile_gif_pal_size
  );

  /* tile settings */
  nkx::setTile(1, 0,
    (uint8_t*)&_binary_tile_gif_tile_start,
    (size_t)  &_binary_tile_gif_tile_size
  );

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
  hwTileRam.tilemap[1].tileIdx[1].data = tileno;
  hwTileRam.tilemap[1].tileIdx[3].data = tileno;
  /* BG settings */
  hwTileRam.bg[0].paletteInfo.mode = 1;
  hwTileRam.bg[0].paletteInfo.bank = 0;
  hwTileRam.bg[0].paletteInfo.no = 0;
  hwTileRam.bg[0].tileBank = 0;
  hwTileRam.bg[0].tilemapBank = 0;
  hwTileRam.bg[0].x = 0;
  hwTileRam.bg[0].y = 0;
  hwTileRam.bg[0].affineEnable = true;
  hwTileRam.bg[0].affineInv.a = -0x0100;
  hwTileRam.bg[0].affineInv.b =  0x0000;
  hwTileRam.bg[0].affineInv.c =  0x0000;
  hwTileRam.bg[0].affineInv.d =  0x0080;
  hwTileRam.bg[0].affineInv.x = HW_SCREEN_W / 2;
  hwTileRam.bg[0].affineInv.y = 0;
  // hwTileRam.bg[1].flag.affineEnable = true;
  // hwTileRam.bg[2].flag.affineEnable = true;
  // hwTileRam.bg[3].flag.affineEnable = true;
  hwTileRam.bg[0].layer = 2;
  hwTileRam.bg[1].layer = 1;
  hwTileRam.bg[2].layer = 0;
  hwTileRam.bg[3].layer = 0;
  hwTileRam.bg[0].enable = true;
  /* SP tile settings */
  /* SP sprite settings */
  hwTileRam.sp[0].sprite[0].enable = true;
  hwTileRam.sp[0].sprite[0].paletteInfo.mode = 1;
  hwTileRam.sp[0].sprite[0].paletteInfo.bank = 0;
  hwTileRam.sp[0].sprite[0].paletteInfo.no = 1;
  hwTileRam.sp[0].sprite[0].tileBank = 1;
  hwTileRam.sp[0].sprite[0].tileIdx = 0;
  hwTileRam.sp[0].sprite[0].tileSize = HW_SPRITE_TILESIZE_64x64;
  hwTileRam.sp[0].sprite[0].x = 32;
  hwTileRam.sp[0].sprite[0].y = 32;
  hwTileRam.sp[0].sprite[0].affineEnable = true;
  hwTileRam.sp[0].sprite[0].affineInv.x = 64 / 2;
  hwTileRam.sp[0].sprite[0].affineInv.y = 64 / 2;

  hwTileRam.sp[0].sprite[1].enable = true;
  hwTileRam.sp[0].sprite[1].paletteInfo.mode = 1;
  hwTileRam.sp[0].sprite[1].paletteInfo.bank = 0;
  hwTileRam.sp[0].sprite[1].paletteInfo.no = 1;
  hwTileRam.sp[0].sprite[1].tileBank = 1;
  hwTileRam.sp[0].sprite[1].tileIdx = 0;
  hwTileRam.sp[0].sprite[1].tileSize = HW_SPRITE_TILESIZE_64x64;
  hwTileRam.sp[0].sprite[1].x = 32;
  hwTileRam.sp[0].sprite[1].y = 152;
  hwTileRam.sp[0].sprite[1].affineEnable = true;
  hwTileRam.sp[0].sprite[1].affineInv.x = 64 / 2;
  hwTileRam.sp[0].sprite[1].affineInv.y = 64 / 2;

  hwTileRam.sp[0].sprite[2].enable = true;
  hwTileRam.sp[0].sprite[2].paletteInfo.mode = 1;
  hwTileRam.sp[0].sprite[2].paletteInfo.bank = 0;
  hwTileRam.sp[0].sprite[2].paletteInfo.no = 1;
  hwTileRam.sp[0].sprite[2].tileBank = 1;
  hwTileRam.sp[0].sprite[2].tileIdx = 0;
  hwTileRam.sp[0].sprite[2].tileSize = HW_SPRITE_TILESIZE_64x64;
  hwTileRam.sp[0].sprite[2].x = 192;
  hwTileRam.sp[0].sprite[2].y = 32;
  hwTileRam.sp[0].sprite[2].affineEnable = true;
  hwTileRam.sp[0].sprite[2].affineInv.x = 64 / 2;
  hwTileRam.sp[0].sprite[2].affineInv.y = 64 / 2;

  /* main loop */
  while (1) {
  }

  return 0;
}
