#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nkx/hw/nkx_hw.h>
#include <nkx/mw/nkx_mw.h>

extern uint8_t _binary_rsc_sysfont8x8_gif_pal_start;
extern uint8_t _binary_rsc_sysfont8x8_gif_pal_size;
extern uint8_t _binary_rsc_sysfont8x8_gif_tile_start;
extern uint8_t _binary_rsc_sysfont8x8_gif_tile_size;

const uint16_t ground_map[16][2] = {
  {4, 5}, {36, 37}, {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99},
  {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99},
};

int32_t camera_x = 0;
int8_t padIdleCount = 0;
bool isVsyncIntr = false;

constexpr int digits = 10+1;
uint32_t countup[4] = {0, 0, 0, 0};
char countupStr[4][digits] = {
  {0},
  {0},
  {0},
  {0}
};

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
  if (intstatus.timer0) {
    intstatus.timer0 = 0;
    countup[0]++;
  }
  if (intstatus.timer1) {
    intstatus.timer1 = 0;
    countup[1]++;
  }
  if (intstatus.timer2) {
    intstatus.timer2 = 0;
    countup[2]++;
  }
  if (intstatus.timer3) {
    intstatus.timer3 = 0;
    countup[3]++;
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
  ioram.intr.enable.bits = (1<<HW_IO_INT_HBLANK) | (1<<HW_IO_INT_VBLANK);
  ioram.intr.enable.bits = (1<<HW_IO_INT_TIMER0) | (1<<HW_IO_INT_TIMER1) | (1<<HW_IO_INT_TIMER2) | (1<<HW_IO_INT_TIMER3);

  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  /* palette settings */
  nkx::loadAsciiFontPalette(
    0, 0,
    (uint32_t*)&_binary_rsc_sysfont8x8_gif_pal_start,
    (size_t)&_binary_rsc_sysfont8x8_gif_pal_size
  );

  /* tile settings */
  nkx::loadAsciiFontTile(
    0,
    (uint8_t*)&_binary_rsc_sysfont8x8_gif_tile_start,
    (size_t)&_binary_rsc_sysfont8x8_gif_tile_size
  );

  /* tilemap settings */

  /* BG settings */
  nkx::setAsciiFontBG(0, 0, 0, 0, 0);

  nkx::putsBG(0, 2, 2, "Timer", 10);

  /* timer settings */
  ioram.timer.timer[0].limit = 1024;
  ioram.timer.timer[1].limit = 3200;  // baud rate 9600
  ioram.timer.timer[2].limit = 65536;
  ioram.timer.timer[3].limit = (320+80)*(240+80)*4*60;  // 1 sec
  for (int i=0; i<4; i++) {
    ioram.timer.timer[i].count = 0;
    ioram.timer.timer[i].repeat = true;
    ioram.timer.timer[i].interrupt = true;
    ioram.timer.timer[i].enable = true;
  }

  nkx::putsBG(0, 2, 4, countupStr[0], digits);
  nkx::putsBG(0, 2, 5, countupStr[1], digits);
  nkx::putsBG(0, 2, 6, countupStr[2], digits);
  nkx::putsBG(0, 2, 7, countupStr[3], digits);

  /* main loop */
  while (1) {
    waitForVSync();

    volatile uint32_t pad = (*(volatile uint32_t*)HWREG_IO_PAD0_ADDR);
    volatile uint16_t scanline = (*(volatile uint16_t*)HWREG_IO_SCANLINE_ADDR);

    for (int i=0; i<4; i++) {
      countupStr[i][0] = '0';
      countupStr[i][digits-1] = '\0';
      uint32_t countupi = countup[i];
      for (int d=digits-2; d>0; d--) {
        countupStr[i][d] = '0' + (countupi % 10);
        countupi /= 10;
        // if (countupi == 0) break;
      }
    }

    nkx::putsBG(0, 2, 4, countupStr[0], digits);
    nkx::putsBG(0, 2, 5, countupStr[1], digits);
    nkx::putsBG(0, 2, 6, countupStr[2], digits);
    nkx::putsBG(0, 2, 7, countupStr[3], digits);
  }

  return 0;
}
