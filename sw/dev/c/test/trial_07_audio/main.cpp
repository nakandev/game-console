#include <nkx/hw/nkx_hw.h>
#include <nkx/sw/nkx_sw.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
void int_handler()
{
  HwTileRam& hwTileRam = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  HwARam& hwARam = *(HwARam*)HWREG_ARAM_BASEADDR;
  volatile uint32_t pad = (*(uint32_t*)0x0300'0000);
  if (pad & 0x4000u) {  // left
    if (hwARam.music[0].masterDirection > -16) {
      hwARam.music[0].masterDirection--;
      hwTileRam.sp[0].sprite[0].x -= 2;
    }
  } else
  if (pad & 0x8000u) {  // right
    if (hwARam.music[0].masterDirection < 16) {
      hwARam.music[0].masterDirection++;
      hwTileRam.sp[0].sprite[0].x += 2;
    }
  }
}
};

void sprite_init()
{
  HwTileRam& hwTileRam = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  /* palette settings */
  hwTileRam.palette[0].color[ 0] = {.data=0x00000000};
  hwTileRam.palette[0].color[ 1] = {.data=0x000000ff};
  hwTileRam.palette[0].color[ 2] = {.data=0xffffffff};
  /* tile settings */
  int sptileNo = 15;
  int tidx = 0;
  for (int i=0; i<8; i++) {
    hwTileRam.tile[sptileNo][tidx].data[0][i] = 2;
    hwTileRam.tile[sptileNo][tidx].data[7][i] = 2;
    hwTileRam.tile[sptileNo][tidx].data[i][0] = 2;
    hwTileRam.tile[sptileNo][tidx].data[i][7] = 2;
  }
  /* SP sprite settings */
  hwTileRam.sp[0].sprite[0].flag.enable = true;
  hwTileRam.sp[0].sprite[0].paletteNo = 0;
  hwTileRam.sp[0].sprite[0].tileNo = sptileNo;
  hwTileRam.sp[0].sprite[0].tileIdx = tidx;
  hwTileRam.sp[0].sprite[0].tileSize = HW_SPRITE_TILESIZE_8x8;
  hwTileRam.sp[0].sprite[0].x = HW_SCREEN_W / 2 - 4;
  hwTileRam.sp[0].sprite[0].y = HW_SCREEN_H / 2 - 4;
}

int main()
{
  sprite_init();
  HwARam& hwARam = *(HwARam*)HWREG_ARAM_BASEADDR;
  /* soundop settings */
  hwARam.soundOp[1].func = HW_SOUNDOP_SIN;
  hwARam.soundOp[1].arg = 0x0f;
  hwARam.soundOp[1].A = 0x01;
  hwARam.soundOp[1].D = 0x1f;
  hwARam.soundOp[1].S = 0x03;
  hwARam.soundOp[1].R = 0x04;
  hwARam.soundOp[1].amp = 1*16 + 0;
  hwARam.soundOp[1].ratio = 49;
  hwARam.soundOp[1].detune = 0;
  hwARam.soundOp[2].func = HW_SOUNDOP_SIN;
#if 1
  hwARam.soundOp[2].arg = 0x0f;
  hwARam.soundOp[2].A = 0x1f;
  hwARam.soundOp[2].D = 0x1f;
  hwARam.soundOp[2].S = 0x00;
  hwARam.soundOp[2].R = 0x08;
  hwARam.soundOp[2].amp = 10*16 + 0;
  hwARam.soundOp[2].ratio = 0;
  hwARam.soundOp[2].detune = 10;
  hwARam.instrument[0].algorithm = 3;
#else
  hwARam.soundOp[2].arg = 0;
  hwARam.soundOp[2].A = 0x00;
  hwARam.soundOp[2].D = 0x1f;
  hwARam.soundOp[2].S = 0x03;
  hwARam.soundOp[2].R = 0x04;
  hwARam.soundOp[2].amp = 0*16 + 12;
  hwARam.soundOp[2].ratio = 61;
  hwARam.soundOp[2].detune = 0;
  hwARam.instrument[0].algorithm = 4;
#endif
  /* instrument settings */
  hwARam.instrument[0].soundOpId[0] = 1;
  hwARam.instrument[0].soundOpId[1] = 2;
  /* musicsheet settings */
  hwARam.musicsheet[0].note[ 0] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start= 0*4};
  hwARam.musicsheet[0].note[ 1] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-7, .start= 1*4};
  hwARam.musicsheet[0].note[ 2] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start= 2*4};
  hwARam.musicsheet[0].note[ 3] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-4, .start= 3*4};
  hwARam.musicsheet[0].note[ 4] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start= 4*4};
  hwARam.musicsheet[0].note[ 5] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-7, .start= 5*4};
  hwARam.musicsheet[0].note[ 6] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start= 6*4};
  hwARam.musicsheet[0].note[ 7] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start= 7*4};
  hwARam.musicsheet[0].note[ 8] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start= 8*4};
  hwARam.musicsheet[0].note[ 9] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-4, .start= 9*4};
  hwARam.musicsheet[0].note[10] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-2, .start=10*4};
  hwARam.musicsheet[0].note[11] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=11*4};
  hwARam.musicsheet[0].note[12] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-2, .start=12*4};
  hwARam.musicsheet[0].note[13] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-4, .start=13*4};
  hwARam.musicsheet[0].note[14] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start=14*4};
  hwARam.musicsheet[0].note[15] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=15*4};
  hwARam.musicsheet[0].note[16] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=16*4};
  hwARam.musicsheet[0].note[17] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=17*4};
  hwARam.musicsheet[0].note[18] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=18*4};
  hwARam.musicsheet[0].note[19] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=19*4};
  hwARam.musicsheet[0].note[20] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=20*4};
  hwARam.musicsheet[0].note[21] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=21*4};
  hwARam.musicsheet[0].note[22] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=22*4};
  hwARam.musicsheet[0].note[23] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=23*4};
  hwARam.musicsheet[0].note[24] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=24*4};
  hwARam.musicsheet[0].note[25] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=24*4+2};
  hwARam.musicsheet[0].note[26] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-7, .start=25*4};
  hwARam.musicsheet[0].note[27] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-7, .start=25*4+2};
  hwARam.musicsheet[0].note[28] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start=26*4};
  hwARam.musicsheet[0].note[29] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start=26*4+2};
  hwARam.musicsheet[0].note[30] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-4, .start=27*4};
  hwARam.musicsheet[0].note[31] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-4, .start=27*4+2};
  hwARam.musicsheet[0].note[32] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-5, .start=28*4};
  hwARam.musicsheet[0].note[33] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-7, .start=29*4};
  hwARam.musicsheet[0].note[34] = {.enable=true,  .instrumentId=0, .length=3, .channel=0, .direction=0, .scale=-9, .start=30*4};
  hwARam.musicsheet[0].note[35] = {.enable=false, .instrumentId=0, .length=3, .channel=0, .direction=0, .scale= 0, .start=31*4};

  /* music settings */
  hwARam.music[0].flag.loop = false;
  hwARam.music[0].loopNote = 0;
  hwARam.music[0].endNote = 32*4;
  hwARam.music[0].noteFrameLength = HW_MUSIC_DEFAULT_NOTE_FRAMELEN / 8;
  hwARam.music[0].channel[0].sheetId = 0;
  hwARam.music[0].channel[0].enable = true;
  hwARam.music[0].masterVolume = 256*64;
  hwARam.music[0].flag.enable = true;

  /* main loop */
  while (1) {
  }

  return 0;
}
