#include <nkx/hw/nkx_hw.h>
#include <nkx/sw/nkx_sw.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
void int_handler()
{
  HwARam& hwARam = *(HwARam*)HWREG_ARAM_BASEADDR;
  volatile uint32_t pad = (*(uint32_t*)0x0300'0000);
  if (pad & 0x4000u) {  // left
    for (int n=0; n<8; n++) {
      if (hwARam.musicsheet[0].note[n].direction > -16)
        hwARam.musicsheet[0].note[n].direction--;
    }
  } else
  if (pad & 0x8000u) {  // right
    for (int n=0; n<8; n++) {
      if (hwARam.musicsheet[0].note[n].direction < 16)
        hwARam.musicsheet[0].note[n].direction++;
    }
  }
}
};

int main()
{
  HwARam& hwARam = *(HwARam*)HWREG_ARAM_BASEADDR;
  /* instrument settings */
  for (int n=0; n<8; n++) {
    for (int t=0; t<16; t++) {
      const int first = 1000;
      int amp = t*(-first/16) + first;
      if (amp < 0) amp = 0;
      hwARam.instrument[n].spectrum[t][0] = HwSpectrum(48, amp);
      hwARam.instrument[n].spectrum[t][1] = HwSpectrum(60, amp/2);
      hwARam.instrument[n].spectrum[t][2] = HwSpectrum(66, amp/5);
      hwARam.instrument[n].spectrum[t][3] = HwSpectrum(72, amp/20);
      hwARam.instrument[n].spectrum[t][4] = HwSpectrum(78, amp/30);
      hwARam.instrument[n].spectrum[t][5] = HwSpectrum(82, amp/50);
    }
  }
  /* musicsheet settings */
  for (int n=0; n<8; n++) {
    hwARam.musicsheet[0].note[n] = {
      .enable=true, .instrumentId=(uint8_t)n, .channel=0, .direction=0, .scale=(int8_t)(n*2)
    };
  }

  /* music settings */
  hwARam.music[0].flag.loop = true;
  hwARam.music[0].loopNote = 0;
  hwARam.music[0].endNote = 8;
  hwARam.music[0].noteFrameLength = HW_MUSIC_DEFAULT_NOTE_FRAMELEN;
  hwARam.music[0].channel[0].sheetId = 0;
  hwARam.music[0].channel[0].enable = true;
  hwARam.music[0].flag.enable = true;

  /* main loop */
  while (1) {
  }

  return 0;
}
