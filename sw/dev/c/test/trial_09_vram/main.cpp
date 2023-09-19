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
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  uint8_t* vram = (uint8_t*)HWREG_VRAM_BASEADDR;
  HwColor* pixels = (HwColor*)vram;
  for (int y=0; y<HW_SCREEN_H; y++) {
    for (int x=0; x<HW_SCREEN_W; x++) {
      uint8_t r = x*255/HW_SCREEN_W;
      uint8_t g = y*255/HW_SCREEN_H;
      pixels[x + y * HW_SCREEN_W] = HwColor{.a=255, .b=0, .g=g, .r=r};
    }
  }

  /* BG settings */
  tileram.bg[0].flag.mode = HWBG_PIXEL_MODE;

  /* main loop */
  while (1) {
  }

  return 0;
}
