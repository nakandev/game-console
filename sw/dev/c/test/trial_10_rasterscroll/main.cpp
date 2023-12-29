#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nkx/hw/nkx_hw.h>
#include <nkx/mw/nkx_mw.h>

extern uint8_t _binary_riscv_logo_gif_pal_start;
extern uint8_t _binary_riscv_logo_gif_pal_size;
extern uint8_t _binary_riscv_logo_gif_tile_start;
extern uint8_t _binary_riscv_logo_gif_tile_size;

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
uint16_t radV = 0;
uint8_t mode = 0;
void int_handler()
{
  HwTileRam& tileram = (*(HwTileRam*)HWREG_TILERAM_BASEADDR);
  volatile uint32_t intstatus = (*(volatile uint32_t*)HWREG_IO_INT_STATUS_ADDR);
  volatile uint16_t scanline = (*(volatile uint16_t*)HWREG_IO_SCANLINE_ADDR);
  volatile uint32_t pad = (*(volatile uint32_t*)HWREG_IO_PAD0_ADDR);
  if (pad & (1 << HW_PAD_A)) {
    mode = 0;
  } else
  if (pad & (1 << HW_PAD_B)) {
    mode = 1;
  }

  if (mode == 0) {
    tileram.bg[0].x = nkx::sin((scanline + radV) & 0xffu) / 8;
    tileram.bg[0].y = 0;
  } else {
    tileram.bg[0].x = 0;
    tileram.bg[0].y = nkx::sin((scanline + radV) & 0xffu) / 8;
  }

  if (intstatus & (1<<HW_IO_INT_HBLANK)) {
  }
  if (intstatus & (1<<HW_IO_INT_VBLANK)) {
      radV = (radV + 1) & 0xffu;
  }
}
};

int main()
{
  HwIoRam& ioram = *(HwIoRam*)HWREG_IORAM_BASEADDR;
  ioram.intr.enable.bits = (1<<HW_IO_INT_HBLANK) | (1<<HW_IO_INT_VBLANK);

  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  /* palette settings */
  nkx::setPalette(0, 1, 0,
    (uint32_t*)&_binary_riscv_logo_gif_pal_start,
    (size_t)   &_binary_riscv_logo_gif_pal_size
  );
  tileram.palette[0].color[1*16] = {.a=0xff, .b=0xff, .g=0xff, .r=0xff};

  /* tile settings */
  nkx::setTile(1, 0,
    (uint8_t*)&_binary_riscv_logo_gif_tile_start,
    (size_t)  &_binary_riscv_logo_gif_tile_size
  );

  /* tilemap settings */
  nkx::setTilemap2DLambda(1,
      HW_TILEMAP_XTILE, HW_TILEMAP_YTILE,
      4, 2,
      [](int32_t x, int32_t y){return (uint16_t)(x + y*32);},
      32, 32,
      0, 0, 32, 25
  );

  /* BG settings */
  tileram.bg[0].flag.enable = true;
  tileram.bg[0].paletteInfo.mode = 1;
  tileram.bg[0].paletteInfo.bank = 0;
  tileram.bg[0].paletteInfo.no = 1;
  tileram.bg[0].tileNo = 1;
  tileram.bg[0].tilemapNo = 1;
  tileram.bg[0].x = 0;
  tileram.bg[0].y = 0;

  /* main loop */
  while (1) {
  }

  return 0;
}
