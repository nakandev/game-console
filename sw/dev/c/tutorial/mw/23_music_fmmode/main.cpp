#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nkx/hw/nkx_hw.h>
#include <nkx/mw/nkx_mw.h>

extern uint8_t _binary_rsc_sysfont8x8_gif_pal_start;
extern uint8_t _binary_rsc_sysfont8x8_gif_pal_size;
extern uint8_t _binary_rsc_sysfont8x8_gif_tile_start;
extern uint8_t _binary_rsc_sysfont8x8_gif_tile_size;

extern uint8_t _binary_rsc_piano_gif_pal_start;
extern uint8_t _binary_rsc_piano_gif_pal_size;
extern uint8_t _binary_rsc_piano_gif_tile_start;
extern uint8_t _binary_rsc_piano_gif_tile_size;

const uint16_t ground_map[16][2] = {
  {4, 5}, {36, 37}, {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99},
  {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99},
};

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

void init_piano_image()
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  /* palette settings */
  nkx::setPalette(0, 1, 0,
    (uint32_t*)&_binary_rsc_piano_gif_pal_start,
    (size_t)   &_binary_rsc_piano_gif_pal_size
  );

  /* tile settings */
  nkx::setTile(1, 0,
    (uint8_t*)&_binary_rsc_piano_gif_tile_start,
    (size_t)  &_binary_rsc_piano_gif_tile_size
  );

  for (int i=0; i<7; ++i) {
    nkx::setTilemap2DLambda(1,
        HW_TILEMAP_XTILE, HW_TILEMAP_YTILE,
        0 + i * 7, 24,
        [](int32_t x, int32_t y){return (uint16_t)((x) + y*8);},
        8, 4,
        1, 0, 7, 4
    );
  }

  /* BG settings */
  // tileram.bg[2].layer = 2;
  tileram.bg[3].paletteInfo.mode = HW_BG_PIXEL_MODE;
  tileram.bg[3].paletteInfo.bank = 0;
  tileram.bg[3].paletteInfo.no = 1;
  tileram.bg[3].tileBank = 1;
  tileram.bg[3].tilemapBank = 1;
  tileram.bg[3].x = 0;
  tileram.bg[3].y = 0;
  tileram.bg[3].enable = true;
}

void init_text()
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  /* palette settings */
  nkx::loadAsciiFontPalette(0, 0,
    (uint32_t*)&_binary_rsc_sysfont8x8_gif_pal_start,
    (size_t)&_binary_rsc_sysfont8x8_gif_pal_size
  );
  nkx::loadAsciiFontTile(0,
    (uint8_t*)&_binary_rsc_sysfont8x8_gif_tile_start,
    (size_t)&_binary_rsc_sysfont8x8_gif_tile_size
  );

  /* tilemap settings */
  /* BG settings */
  nkx::setAsciiFontBG(0, 0, 0, 0, 0);

  nkx::putsBG(0, 2, 2, "A:START", 20);
  nkx::putsBG(0, 2, 3, "B:STOP", 20);
  nkx::putsBG(0, 2, 4, "D:RESET", 20);
}

void init_music_data()
{
  HwInstRam& instram = *(HwInstRam*)HWREG_INSTRAM_BASEADDR;
  /* soundop settings */
  instram.soundOp[1].func = HW_SOUNDOP_SIN;
  instram.soundOp[1].arg = 0x0f;
  instram.soundOp[1].A = 0x01;
  instram.soundOp[1].D = 0x1f;
  instram.soundOp[1].S = 0x03;
  instram.soundOp[1].R = 0x04;
  instram.soundOp[1].amp = 1*16 + 0;
  instram.soundOp[1].ratio = 49;
  instram.soundOp[1].detune = 0;
  instram.soundOp[2].func = HW_SOUNDOP_SIN;
  instram.soundOp[2].arg = 0;
  instram.soundOp[2].A = 0x00;
  instram.soundOp[2].D = 0x1f;
  instram.soundOp[2].S = 0x03;
  instram.soundOp[2].R = 0x04;
  instram.soundOp[2].amp = 0*16 + 12;
  instram.soundOp[2].ratio = 61;
  instram.soundOp[2].detune = 0;
  instram.instrument[0].algorithm = 4;
  /* instrument settings */
  instram.instrument[0].soundOpId[0] = 1;
  instram.instrument[0].soundOpId[1] = 2;
  /* musicsheet settings */
  instram.musicsheet[0].note[ 0] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start= 0*4};
  instram.musicsheet[0].note[ 1] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-7, .start= 1*4};
  instram.musicsheet[0].note[ 2] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start= 2*4};
  instram.musicsheet[0].note[ 3] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-4, .start= 3*4};
  instram.musicsheet[0].note[ 4] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start= 4*4};
  instram.musicsheet[0].note[ 5] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-7, .start= 5*4};
  instram.musicsheet[0].note[ 6] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start= 6*4};
  instram.musicsheet[0].note[ 7] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start= 7*4};
  instram.musicsheet[0].note[ 8] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start= 8*4};
  instram.musicsheet[0].note[ 9] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-4, .start= 9*4};
  instram.musicsheet[0].note[10] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-2, .start=10*4};
  instram.musicsheet[0].note[11] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=11*4};
  instram.musicsheet[0].note[12] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-2, .start=12*4};
  instram.musicsheet[0].note[13] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-4, .start=13*4};
  instram.musicsheet[0].note[14] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start=14*4};
  instram.musicsheet[0].note[15] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=15*4};
  instram.musicsheet[0].note[16] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=16*4};
  instram.musicsheet[0].note[17] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=17*4};
  instram.musicsheet[0].note[18] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=18*4};
  instram.musicsheet[0].note[19] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=19*4};
  instram.musicsheet[0].note[20] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=20*4};
  instram.musicsheet[0].note[21] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=21*4};
  instram.musicsheet[0].note[22] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=22*4};
  instram.musicsheet[0].note[23] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=23*4};
  instram.musicsheet[0].note[24] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=24*4};
  instram.musicsheet[0].note[25] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=24*4+2};
  instram.musicsheet[0].note[26] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-7, .start=25*4};
  instram.musicsheet[0].note[27] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-7, .start=25*4+2};
  instram.musicsheet[0].note[28] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start=26*4};
  instram.musicsheet[0].note[29] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start=26*4+2};
  instram.musicsheet[0].note[30] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-4, .start=27*4};
  instram.musicsheet[0].note[31] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-4, .start=27*4+2};
  instram.musicsheet[0].note[32] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start=28*4};
  instram.musicsheet[0].note[33] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-7, .start=29*4};
  instram.musicsheet[0].note[34] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=30*4};
  instram.musicsheet[0].note[35] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=31*4};

  /* music settings */
  instram.music[0].flag.loop = true;
  instram.music[0].loopNote = 0;
  instram.music[0].endNote = 32*4;
  instram.music[0].noteFrameLength = HW_MUSIC_DEFAULT_NOTE_FRAMELEN / 8;
  instram.music[0].channel[0].sheetId = 0;
  instram.music[0].channel[0].enable = true;
  instram.music[0].masterVolume = 256*64;
  instram.music[0].masterDirection = 0;
  instram.music[0].flag.enable = true;
}

int main()
{
  HwIoRam& ioram = *(HwIoRam*)HWREG_IORAM_BASEADDR;
  // ioram.intr.enable.hblank = true;
  ioram.intr.enable.vblank = true;
  // ioram.intr.enable.dma0 = true;
  // ioram.intr.enable.dma1 = true;
  // ioram.intr.enable.dma2 = true;
  // ioram.intr.enable.dma3 = true;
  // ioram.intr.enable.timer0 = true;
  // ioram.intr.enable.timer1 = true;
  // ioram.intr.enable.timer3 = true;
  // ioram.intr.enable.timer4 = true;

  init_piano_image();
  init_text();
  init_music_data();

  /* main loop */
  while (1) {
    waitForVSync();

    volatile uint32_t pad = (*(volatile uint32_t*)HWREG_IO_PAD0_ADDR);
    volatile uint16_t scanline = (*(volatile uint16_t*)HWREG_IO_SCANLINE_ADDR);

    if (pad == 0) {
    } else {
      if (pad & (1 << HW_PAD_B)) {
      }
      if (pad & (1 << HW_PAD_LEFT)) {
      } else
      if (pad & (1 << HW_PAD_RIGHT)) {
      }
    }
  }

  return 0;
}
