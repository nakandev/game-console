#include <apu.h>
#include <memory.h>
#include <io.h>
#include <cmath>
#include <fmt/core.h>

static float spectrum_freqs[HW_SPECTRUM_MAX];

Apu::Apu(Memory& memory)
  : memory(memory),
  musicBuffer(),
  apuMusicData()
{
  musicBuffer.resize(HW_MUSIC_FREQ_PER_FRAME * 2);
  apuMusicData = {.noteTime=0, .frameTime=0, .buffer=musicBuffer.data()};
  for (int i=0; i<HW_SPECTRUM_MAX; i++) {
    spectrum_freqs[i] = 27.5 * powf(2.0, (float)i/12.0);
  }
}

Apu::~Apu()
{
  musicBuffer.clear();
}

static void writeMusicBuffer(ApuMusicData& apudata, HwARam& aram, HwMusic& music, int ch, uint32_t time, float& value)
{
  // auto& music = aram.music[0];
  // auto& noteTime = apudata.noteTime;
  // auto& frameTime = apudata.frameTime;
}

void Apu::updateMusicBuffer()
{
  using FLOAT = double;
  HwARam& aram = *(HwARam*)memory.section("aram").buffer();
  auto& music = aram.music[0];
  auto& noteTime = apuMusicData.noteTime;
  auto& frameTime = apuMusicData.frameTime;
  auto noteId = noteTime;
  for (int l=0; l<HW_MUSIC_FREQ_PER_FRAME; l++) {
    int phase = l + frameTime * HW_MUSIC_FREQ_PER_FRAME;
    int sliceId = phase * HW_INSTRUMENT_TIMESLICE / HW_MUSIC_FREQUENCY % HW_INSTRUMENT_TIMESLICE;
    FLOAT vfL = 0.0;
    FLOAT vfR = 0.0;
    for (int ch=0; ch<8; ch++) {
      if (!music.channel[ch].enable) continue;
      auto sheetId = music.channel[ch].sheetId;
      auto& sheet = aram.musicsheet[sheetId];
      auto& note = sheet.note[noteId];
      if (!note.enable) continue;
      auto instId = note.instrumentId;
      auto& inst = aram.instrument[instId];
      FLOAT dir = note.direction;
      for (int sp=0; sp<HW_SPECTRUM_NUM; sp++) {
        auto spectrum = inst.spectrum[sliceId][sp];
        FLOAT freq = spectrum_freqs[(spectrum.freq + note.scale) % HW_SPECTRUM_MAX];
        FLOAT sinphase = phase * freq * (2.0 * 3.14159265 / (FLOAT)HW_MUSIC_FREQUENCY);
        FLOAT spvf = sinf(sinphase) * spectrum.amp;
        // vfL += spvf * (dir <= 0 ? 1 : (16-dir)/16.0);
        // vfR += spvf * (dir >= 0 ? 1 : (16+dir)/16.0);
        FLOAT dirlen = HW_MUSICNOTE_DIRECTION_LEN;
        vfL += spvf * (dirlen-dir)/(dirlen*2.0);
        vfR += spvf * (dir+dirlen)/(dirlen*2.0);
        // if (sp==0)
        // fmt::print("phase={} note={} instId={} slice={} sp.freq={} spec.amp={} vfL={}\n",
        //     phase, noteId, instId, sliceId, spectrum.freq, spectrum.amp, vfL);
      }
    }
    // fmt::print("{}\n", vfL);
    musicBuffer[l*2+0] = (int16_t)vfL;  // Left-ch
    musicBuffer[l*2+1] = (int16_t)vfR;  // Right-ch
  }
  frameTime++;
  noteTime = frameTime / music.noteFrameLength;
  if (noteTime == music.endNote) {
    frameTime = 0;
    noteTime = 0;
  }
}
