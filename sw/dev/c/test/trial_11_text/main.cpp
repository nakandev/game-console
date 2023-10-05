#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nkx/hw/nkx_hw.h>
#include <nkx/sw/nkx_sw.h>

extern uint8_t _binary_img_sysfont8x8_gif_pal_start;
extern uint8_t _binary_img_sysfont8x8_gif_pal_size;
extern uint8_t _binary_img_sysfont8x8_gif_tile_start;
extern uint8_t _binary_img_sysfont8x8_gif_tile_size;

extern "C" void int_handler()
{
}

int main()
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  /* palette settings */
  nkx::loadAsciiFontPalette(0,
    (uint32_t*)&_binary_img_sysfont8x8_gif_pal_start,
    (size_t)&_binary_img_sysfont8x8_gif_pal_size
  );
  nkx::loadAsciiFontTile(0,
    (uint8_t*)&_binary_img_sysfont8x8_gif_tile_start,
    (size_t)&_binary_img_sysfont8x8_gif_tile_size
  );

  /* tilemap settings */
  /* BG settings */
  nkx::setAsciiFontBG(0, 0, 0, 0);

  nkx::putsBG(0, 2, 2, "Hello World!!", 20);
  nkx::putsBG(0, 2, 3, "abcdefg", 20);
  nkx::putsBG(0, 2, 4, "01234567890", 20);

  /* main loop */
  while (1) {
  }

  return 0;
}
