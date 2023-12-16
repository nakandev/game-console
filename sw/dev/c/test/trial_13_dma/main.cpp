#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nkx/hw/nkx_hw.h>
#include <nkx/sw/nkx_sw.h>

extern "C" void int_handler()
{
  HwTileRam& tileram = (*(HwTileRam*)HWREG_TILERAM_BASEADDR);
  volatile uint32_t intstatus = (*(volatile uint32_t*)HWREG_IO_INT_STATUS_ADDR);
  volatile uint16_t scanline = (*(volatile uint16_t*)HWREG_IO_SCANLINE_ADDR);
  volatile uint32_t pad = (*(volatile uint32_t*)HWREG_IO_PAD0_ADDR);

  if (intstatus & (1<<HW_IO_INT_HBLANK)) {
  }
  if (intstatus & (1<<HW_IO_INT_VBLANK)) {
  }
}

const HwColor hsv2rgb(uint8_t h, uint8_t s, uint8_t v)
{
  HwColor rgb{.a=255, .b=0, .g=0, .r=0};
  if (s == 0) {
    rgb.r = rgb.g = rgb.b = v;
  }
  else {
    int _max = v;
    int _min = _max - (s * _max / 255);
    if ((0*255/6) <= h && h < (1*255/6)) {
      rgb.r = _max;
      rgb.g = (h) * (_max - _min) / 43 + _min;
      rgb.b = _min;
    } else
    if ((1*255/6) <= h && h < (2*255/6)) {
      rgb.r = ((85 - h)) * (_max - _min) / 43 + _min;
      rgb.g = _max;
      rgb.b = _min;
    } else
    if ((2*255/6) <= h && h < (3*255/6)) {
      rgb.r = _min;
      rgb.g = _max;
      rgb.b = ((h - 85)) * (_max - _min) / 43 + _min;
    } else
    if ((3*255/6) <= h && h < (4*255/6)) {
      rgb.r = _min;
      rgb.g = ((170 - h)) * (_max - _min) / 43 + _min;
      rgb.b = _max;
    } else
    if ((4*255/6) <= h && h < (5*255/6)) {
      rgb.r = ((h - 170)) * (_max - _min) / 43 + _min;
      rgb.g = _min;
      rgb.b = _max;
    } else
    {
      rgb.r = _max;
      rgb.g = _min;
      rgb.b = ((255 - h)) * (_max - _min) / 43 + _min;
    }
  }
  return rgb;
}

int dummyCount = 0;
int main()
{
  HwIoRam& ioram = *(HwIoRam*)HWREG_IORAM_BASEADDR;
  ioram.intr.enable = (1<<HW_IO_INT_HBLANK) | (1<<HW_IO_INT_VBLANK);

  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  uint8_t* vram = (uint8_t*)HWREG_VRAM_BASEADDR;

  /* Work RAM init */
  HwColor* pixels = (HwColor*)HWREG_FASTWORKRAM_BASEADDR;
  for (int y=0; y<HW_SCREEN_H; y++) {
    for (int x=0; x<HW_SCREEN_W; x++) {
      uint8_t h = x*255/HW_SCREEN_W;
      uint8_t s = 255 - y*255/HW_SCREEN_H;
      uint8_t v = 255;
      pixels[x + y * HW_SCREEN_W] = hsv2rgb(h, s, v);
    }
  }

  /* DMA transfer */
  ioram.dma.dma[0].src = HWREG_FASTWORKRAM_BASEADDR;
  ioram.dma.dma[0].dst = HWREG_VRAM_BASEADDR + HW_SCREEN_W * 0 * sizeof(HwColor);
  ioram.dma.dma[0].size = HW_IO_DMA_SIZE_32BIT;
  ioram.dma.dma[0].count = HW_SCREEN_W * 16;
  ioram.dma.dma[0].repeat = false;
  ioram.dma.dma[0].interrupt = false;
  ioram.dma.dma[0].mode = HW_IO_DMA_MODE_BURST;
  ioram.dma.dma[0].srcIncrement = HW_IO_DMA_INCREMENT_INC;
  ioram.dma.dma[0].dstIncrement = HW_IO_DMA_INCREMENT_INC;
  ioram.dma.dma[0].trigger = HW_IO_DMA_TRIGGER_IMMEDIATELY;

  ioram.dma.dma[1].src = HWREG_FASTWORKRAM_BASEADDR;
  ioram.dma.dma[1].dst = HWREG_VRAM_BASEADDR + HW_SCREEN_W * 32 * sizeof(HwColor);
  ioram.dma.dma[1].size = HW_IO_DMA_SIZE_32BIT;
  ioram.dma.dma[1].count = HW_SCREEN_W * 16;
  ioram.dma.dma[1].repeat = false;
  ioram.dma.dma[1].interrupt = false;
  ioram.dma.dma[1].mode = HW_IO_DMA_MODE_BURST;
  ioram.dma.dma[1].srcIncrement = HW_IO_DMA_INCREMENT_INC;
  ioram.dma.dma[1].dstIncrement = HW_IO_DMA_INCREMENT_INC;
  ioram.dma.dma[1].trigger = HW_IO_DMA_TRIGGER_IMMEDIATELY;

  ioram.dma.dma[0].enable = true;
  while(dummyCount++ < 100) ;
  ioram.dma.dma[1].enable = true;
  // enable flags will be 'false' when DMA transfer is finished.

  /* BG settings */
  tileram.bg[0].flag.enable = true;
  tileram.bg[0].flag.mode = HWBG_PIXEL_MODE;

  /* main loop */
  while (1) {
  }

  return 0;
}
