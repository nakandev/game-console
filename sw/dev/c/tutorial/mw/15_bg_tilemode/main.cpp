#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nkx/hw/nkx_hw.h>
#include <nkx/mw/nkx_mw.h>

bool isVsyncIntr = false;

extern "C" void int_handler()
{
  // [TODO] Should the status flag be accessed each time it is checked?
  volatile HwIoInterruptBits& intstatus = (*(volatile HwIoInterruptBits*)HWREG_IO_INT_STATUS_ADDR);

  if (intstatus.hblank) {
    intstatus.hblank = 0;
  }
  if (intstatus.vblank) {
    intstatus.vblank = 0;
    isVsyncIntr = true;
  }
}

void waitForVSync()
{
  while (!isVsyncIntr) {
    asm("nop");
    asm("wfi");
    asm("nop");
  }
  isVsyncIntr = false;
}

int main()
{
  HwIoRam& ioram = *(HwIoRam*)HWREG_IORAM_BASEADDR;
  ioram.intr.enable.hblank = true;
  ioram.intr.enable.vblank = true;
  // ioram.intr.enable.dma0 = true;
  // ioram.intr.enable.dma1 = true;
  // ioram.intr.enable.dma2 = true;
  // ioram.intr.enable.dma3 = true;
  // ioram.intr.enable.timer0 = true;
  // ioram.intr.enable.timer1 = true;
  // ioram.intr.enable.timer3 = true;
  // ioram.intr.enable.timer4 = true;

  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  /* palette settings */
  tileram.palette[0].data[0] = 0x00000000u;
  for (int i=0; i<8; i++) {
    for (int t=0; t<4; t++) {
      int r=0, g=0, b=0;
      switch (i) {
        default:
        case 0: r=1; g=0; b=0; break;
        case 1: r=0; g=1; b=0; break;
        case 2: r=0; g=0; b=1; break;
        case 3: r=1; g=1; b=0; break;
        case 4: r=1; g=0; b=1; break;
        case 5: r=0; g=1; b=1; break;
        case 6: r=0; g=0; b=0; break;
        case 7: r=1; g=1; b=1; break;
      }
      tileram.palette[0].color[i*4 + t + 1] = {
        .a=(uint8_t)(0xffu),
        .b=(uint8_t)((b*0x3f*(t+1))&0xffu),
        .g=(uint8_t)((g*0x3f*(t+1))&0xffu),
        .r=(uint8_t)((r*0x3f*(t+1))&0xffu),
      };
    }
  }
  tileram.palette[0].data[33] = 0x1f00ffffu;

  /* tile settings */
  for (int m=0; m<8; m++) {
    for (int t=0; t<4; t++) {  // 16x16 -> 4-tiles
      for (int p=0; p<64; p++) {
        int px = p % 8, py = p / 8;
        tileram.tile[0][m*4 + t].data[py][px] = m*4 + t + 1;
      }
    }
  }

  for (int p=0; p<64; p++) {
    int px = p % 8, py = p / 8;
    tileram.tile[1][0].data[py][px] = 0;
    tileram.tile[1][1].data[py][px] = 33;
  }

  /* tilemap settings */
  for (int i=0; i<1024; i++) {
    // tileram.tilemap[0].tileIdx[i].data = i % 8;
    tileram.tilemap[0].tileIdx[i].idx = i % 8;
  }

  // tileram.tilemap[1].tileIdx[0].data = 0;
  for (int ty=20; ty<30; ty++) {
    for (int tx=2; tx<40-2; tx++) {
      int tOffset = ty*64 + tx;
      // tileram.tilemap[1].tileIdx[tOffset].data = 1;
      tileram.tilemap[1].tileIdx[tOffset].idx = 1;
    }
  }

  /* BG settings */
  tileram.bg[0].enable = true;
  tileram.bg[0].layer = 0;
  tileram.bg[0].paletteInfo.mode = HW_PALETTE_MODE_256;
  tileram.bg[0].paletteInfo.bank = 0;
  tileram.bg[0].paletteInfo.no = 0;
  tileram.bg[0].paletteInfo.transparency = 0;
  tileram.bg[0].tileBank = 0;
  tileram.bg[0].tileSize = HW_SPRITE_TILESIZE_16x16;
  tileram.bg[0].tilemapBank = 0;
  tileram.bg[0].x = 0;
  tileram.bg[0].y = 0;

  tileram.bg[3].enable = true;
  tileram.bg[3].layer = 3;
  tileram.bg[3].paletteInfo.mode = HW_PALETTE_MODE_256;
  tileram.bg[3].paletteInfo.bank = 0;
  tileram.bg[3].paletteInfo.no = 0;
  tileram.bg[3].paletteInfo.transparency = 4;
  tileram.bg[3].tileBank = 1;
  tileram.bg[3].tileSize = HW_SPRITE_TILESIZE_8x8;
  tileram.bg[3].tilemapBank = 1;
  tileram.bg[3].x = 0;
  tileram.bg[3].y = 0;

  /* main loop */
  int wait;
  while (1) {
    waitForVSync();

    volatile uint32_t pad = (*(volatile uint32_t*)HWREG_IO_PAD0_ADDR);
    volatile uint16_t scanline = (*(volatile uint16_t*)HWREG_IO_SCANLINE_ADDR);

    if (pad == 0) {
    }
    if (wait++ == 1) {
      tileram.bg[0].x += 1;
      tileram.bg[0].y += 1;
      wait = 0;
    }
  }

  return 0;
}
