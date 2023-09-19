#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nkx/hw/nkx_hw.h>
#include <nkx/sw/nkx_sw.h>
#include "img_sysfont8x8_gif.h"


extern "C" {
void int_handler()
{
}
};

#define arrayof(x) (sizeof(x)/sizeof(x[0]))
int main()
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  /* palette settings */
  nkx::loadAsciiFontPalette(0, img_sysfont8x8_gif_pal, arrayof(img_sysfont8x8_gif_pal));
  /* tile settings */
  nkx::loadAsciiFontTile(0, img_sysfont8x8_gif_data, arrayof(img_sysfont8x8_gif_data));

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
