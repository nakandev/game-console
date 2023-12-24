#include <apu.h>
#include <memory.h>
#include <io.h>
#include <cmath>
#include <random>
#include <fmt/core.h>

using FLOAT = double;

static FLOAT spectrum_freqs[HW_SPECTRUM_MAX];

std::random_device seed_gen;
std::mt19937 engine(seed_gen());
std::uniform_real_distribution<FLOAT> dist1(-1.0, 1.0);


Apu::Apu(Memory& memory)
  : memory(memory),
  noteBuffer(),
  musicBuffer(),
  apuMusicData(),
  debug()
{
  noteBuffer.resize(HW_MUSIC_CHANNEL_NUM);
  musicBuffer.resize(HW_MUSIC_FREQ_PER_FRAME * 2);
  apuMusicData = {.noteCount=0, .frameCount=0, .buffer=musicBuffer.data()};
  spectrum_freqs[0] = 0.0;
  for (int i=0; i<HW_SPECTRUM_MAX - 1; i++) {
    spectrum_freqs[i + 1] = 27.5 * powf(2.0, (FLOAT)i/12.0);
  }
  debug.enableCh.resize(HW_MUSIC_CHANNEL_NUM);
  for (auto& enable: debug.enableCh)
    enable.v1 = true;
}

Apu::~Apu()
{
  noteBuffer.clear();
  musicBuffer.clear();
}

auto Apu::init() -> void
{
  // noteBuffer.resize(HW_MUSIC_CHANNEL_NUM);
  // musicBuffer.resize(HW_MUSIC_FREQ_PER_FRAME * 2);
  apuMusicData = {.noteCount=0, .frameCount=0, .buffer=musicBuffer.data()};
}

static auto envelope(FLOAT y, HwSoundOp& op, uint32_t noteLength, uint32_t time) -> FLOAT
{
  const uint32_t freq = HW_MUSIC_FREQUENCY;
  FLOAT end = (noteLength + 1) / 4;
  int At = freq * op.A / 0x40;
  int Dt = freq * op.D / 0x40;
  int St = freq * end;
  int Rt = freq * (end + op.R) / 0x40;
  FLOAT Slv = (FLOAT)op.S / 0x40;
  FLOAT k = 0.0;
  if (time < At) {
    k = (FLOAT)time / At;
  } else
  if (time < Dt) {
    k = -(FLOAT)(time - At) / (Dt - At) * (1.0 - Slv) + 1.0;
  } else
  if (time < St) {
    k = Slv;
  } else
  if (time < Rt) {
    k = -(FLOAT)(time - St) / (Rt - Dt) * (Slv) + Slv;
  } else
  {
    k = 0.0;
  }
  return y * k;
}

static auto sig_sin(FLOAT x, FLOAT arg) -> FLOAT
{
  return sin(x);
}

static auto sig_square(FLOAT x, FLOAT arg) -> FLOAT
{
  double dummy;
  FLOAT nx = modf(x / (2 * 3.1415926), &dummy);
  FLOAT y;
  if (nx < arg) {
    y = 1.0;
  } else {
    y = -1.0;
  }
  return y;
}

static auto sig_saw(FLOAT x, FLOAT arg) -> FLOAT {
  double dummy;
  FLOAT nx = modf(x / (2 * 3.1415926), &dummy);
  FLOAT y;
  if (nx <= arg) {
    if (arg == 0)
      y = 1.0;
    else
      y = nx / arg;
  } else
  if (nx < 1 - arg) {
    y = (1 - 2*nx) / (1 - 2*arg);
  } else
  {
    if (arg == 0)
      y = -1.0;
    else
      y = (nx - 1) / arg;
  }
  return y;
}

static auto sig_noise(FLOAT x, FLOAT arg) -> FLOAT
{
  return dist1(engine);
}

static FLOAT (*signalFuncs[])(FLOAT x, FLOAT arg) = {
  sig_sin,
  sig_square,
  sig_saw,
  sig_noise,
};

static auto signalFunc(HwSoundOp& op, int scale, int length, FLOAT time, FLOAT y0) -> FLOAT
{
  FLOAT amp = (FLOAT)op.amp / 16.0;
  FLOAT freq = 0.0;
  FLOAT arg = (FLOAT)op.arg / 32.0;
  FLOAT y;
  if (op.ratio + scale > 0) {
    freq = (FLOAT)spectrum_freqs[op.ratio + scale];
  }
  freq += (FLOAT)op.detune;
  // fmt::print("op.ratio={} scale={} op.detune={} freq={}\n", op.ratio, scale, op.detune, freq);
  y = amp * signalFuncs[op.func](2.0 * 3.14159256 * freq * time / (FLOAT)HW_MUSIC_FREQUENCY + y0, arg);
  y = envelope(y, op, length, time);
  return y;
}

static auto algorithm0(HwInstRam& aram, HwMusicNote& note, uint32_t time) -> FLOAT
{
  /* [3 > 2 > 1 > 0 > [OUT] */
  auto instId = note.instrumentId;
  auto& inst = aram.instrument[instId];
  auto& op0 = aram.soundOp[inst.soundOpId[0]];
  auto& op1 = aram.soundOp[inst.soundOpId[1]];
  auto& op2 = aram.soundOp[inst.soundOpId[2]];
  auto& op3 = aram.soundOp[inst.soundOpId[3]];
  FLOAT y[4];

  y[3] = signalFunc(op3, note.scale, note.length, time, 0.0);
  y[2] = signalFunc(op2, note.scale, note.length, time, y[3]);
  y[1] = signalFunc(op1, note.scale, note.length, time, y[2]);
  y[0] = signalFunc(op0, note.scale, note.length, time, y[1]);

  return y[0];
}

static auto algorithm1(HwInstRam& aram, HwMusicNote& note, uint32_t time) -> FLOAT
{
  /* [3 +        */
  /*  2 +> 1 > 0 > [OUT] */
  auto instId = note.instrumentId;
  auto& inst = aram.instrument[instId];
  auto& op0 = aram.soundOp[inst.soundOpId[0]];
  auto& op1 = aram.soundOp[inst.soundOpId[1]];
  auto& op2 = aram.soundOp[inst.soundOpId[2]];
  auto& op3 = aram.soundOp[inst.soundOpId[3]];
  FLOAT y[4];

  y[3] = signalFunc(op3, note.scale, note.length, time, 0.0);
  y[2] = signalFunc(op2, note.scale, note.length, time, 0.0);
  y[1] = signalFunc(op1, note.scale, note.length, time, y[3] + y[2]);
  y[0] = signalFunc(op0, note.scale, note.length, time, y[1]);

  return y[0];
}

static auto algorithm2(HwInstRam& aram, HwMusicNote& note, uint32_t time) -> FLOAT
{
  /*    [3 +        */
  /* 2 > 1 +> 0 > [OUT] */
  auto instId = note.instrumentId;
  auto& inst = aram.instrument[instId];
  auto& op0 = aram.soundOp[inst.soundOpId[0]];
  auto& op1 = aram.soundOp[inst.soundOpId[1]];
  auto& op2 = aram.soundOp[inst.soundOpId[2]];
  auto& op3 = aram.soundOp[inst.soundOpId[3]];
  FLOAT y[4];

  y[3] = signalFunc(op3, note.scale, note.length, time, 0.0);
  y[2] = signalFunc(op2, note.scale, note.length, time, 0.0);
  y[1] = signalFunc(op1, note.scale, note.length, time, y[2]);
  y[0] = signalFunc(op0, note.scale, note.length, time, y[3] + y[1]);

  return y[0];
}

static auto algorithm3(HwInstRam& aram, HwMusicNote& note, uint32_t time) -> FLOAT
{
  /* [3 > 2 +            */
  /*      1 +> 0 > [OUT] */
  auto instId = note.instrumentId;
  auto& inst = aram.instrument[instId];
  auto& op0 = aram.soundOp[inst.soundOpId[0]];
  auto& op1 = aram.soundOp[inst.soundOpId[1]];
  auto& op2 = aram.soundOp[inst.soundOpId[2]];
  auto& op3 = aram.soundOp[inst.soundOpId[3]];
  FLOAT y[4];

  y[3] = signalFunc(op3, note.scale, note.length, time, 0.0);
  y[2] = signalFunc(op2, note.scale, note.length, time, y[3]);
  y[1] = signalFunc(op1, note.scale, note.length, time, 0.0);
  y[0] = signalFunc(op0, note.scale, note.length, time, y[1]);

  return y[2] + y[0];
}

static auto algorithm4(HwInstRam& aram, HwMusicNote& note, uint32_t time) -> FLOAT
{
  /* 3 > 2 +        */
  /* 1 > 0 +> [OUT] */
  auto instId = note.instrumentId;
  auto& inst = aram.instrument[instId];
  auto& op0 = aram.soundOp[inst.soundOpId[0]];
  auto& op1 = aram.soundOp[inst.soundOpId[1]];
  auto& op2 = aram.soundOp[inst.soundOpId[2]];
  auto& op3 = aram.soundOp[inst.soundOpId[3]];
  FLOAT y[4];

  y[3] = signalFunc(op3, note.scale, note.length, time, 0.0);
  y[2] = signalFunc(op2, note.scale, note.length, time, y[3]);
  y[1] = signalFunc(op1, note.scale, note.length, time, 0.0);
  y[0] = signalFunc(op0, note.scale, note.length, time, y[1]);

  return y[2] + y[0];
}

static auto algorithm5(HwInstRam& aram, HwMusicNote& note, uint32_t time) -> FLOAT
{
  /* [3 > 2 +        */
  /*    > 1 +        */
  /*    > 0 +> [OUT] */
  auto instId = note.instrumentId;
  auto& inst = aram.instrument[instId];
  auto& op0 = aram.soundOp[inst.soundOpId[0]];
  auto& op1 = aram.soundOp[inst.soundOpId[1]];
  auto& op2 = aram.soundOp[inst.soundOpId[2]];
  auto& op3 = aram.soundOp[inst.soundOpId[3]];
  FLOAT y[4];

  y[3] = signalFunc(op3, note.scale, note.length, time, 0.0);
  y[2] = signalFunc(op2, note.scale, note.length, time, y[3]);
  y[1] = signalFunc(op1, note.scale, note.length, time, y[3]);
  y[0] = signalFunc(op0, note.scale, note.length, time, y[3]);

  return y[2] + y[1] + y[0];
}

static auto algorithm6(HwInstRam& aram, HwMusicNote& note, uint32_t time) -> FLOAT
{
  /* [3 > 2 +        */
  /*      1 +        */
  /*      0 +> [OUT] */
  auto instId = note.instrumentId;
  auto& inst = aram.instrument[instId];
  auto& op0 = aram.soundOp[inst.soundOpId[0]];
  auto& op1 = aram.soundOp[inst.soundOpId[1]];
  auto& op2 = aram.soundOp[inst.soundOpId[2]];
  auto& op3 = aram.soundOp[inst.soundOpId[3]];
  FLOAT y[4];

  y[3] = signalFunc(op3, note.scale, note.length, time, 0.0);
  y[2] = signalFunc(op2, note.scale, note.length, time, y[3]);
  y[1] = signalFunc(op1, note.scale, note.length, time, 0.0);
  y[0] = signalFunc(op0, note.scale, note.length, time, 0.0);

  return y[2] + y[1] + y[0];
}

static auto algorithm7(HwInstRam& aram, HwMusicNote& note, uint32_t time) -> FLOAT
{
  /* [3 +        */
  /*  2 +        */
  /*  1 +        */
  /*  0 +> [OUT] */
  auto instId = note.instrumentId;
  auto& inst = aram.instrument[instId];
  auto& op0 = aram.soundOp[inst.soundOpId[0]];
  auto& op1 = aram.soundOp[inst.soundOpId[1]];
  auto& op2 = aram.soundOp[inst.soundOpId[2]];
  auto& op3 = aram.soundOp[inst.soundOpId[3]];
  FLOAT y[4];

  y[3] = signalFunc(op3, note.scale, note.length, time, 0.0);
  y[2] = signalFunc(op2, note.scale, note.length, time, 0.0);
  y[1] = signalFunc(op1, note.scale, note.length, time, 0.0);
  y[0] = signalFunc(op0, note.scale, note.length, time, 0.0);

  return y[3] + y[2] + y[1] + y[0];
}

FLOAT (* const algorithms[])(HwInstRam&, HwMusicNote&, uint32_t) = {
  &algorithm0,
  &algorithm1,
  &algorithm2,
  &algorithm3,
  &algorithm4,
  &algorithm5,
  &algorithm6,
  &algorithm7,
};

auto Apu::updateMusicBuffer() -> void
{
  HwInstRam& aram = *(HwInstRam*)memory.section("inst").buffer();
  auto& music = aram.music[0];
  if (!music.flag.enable) {
    return;
  }
  auto& noteCount = apuMusicData.noteCount;
  auto& frameCount = apuMusicData.frameCount;
  if (frameCount == 0) {
    for (int ch=0; ch<8; ch++) {
      noteBuffer[ch].noteIdx = 0;
      auto sheetId = music.channel[ch].sheetId;
      auto noteId = noteBuffer[ch].noteIdx;
      auto& sheet = aram.musicsheet[sheetId];
      auto& note = sheet.note[0];
      if (music.channel[ch].enable) {
        noteBuffer[ch].note = note;
      } else {
        noteBuffer[ch].note = HwMusicNote{.start=0};
      }
    }
  }
  for (int timeInFrame=0; timeInFrame<HW_MUSIC_FREQ_PER_FRAME; timeInFrame++) {
    if (music.noteFrameLength == 0) {
      break;
    }
    int time = timeInFrame + frameCount * HW_MUSIC_FREQ_PER_FRAME;
    // int timeInSec = time % HW_MUSIC_FREQUENCY;
    FLOAT vfL = 0.0;
    FLOAT vfR = 0.0;
    for (int ch=0; ch<8; ch++) {
      if (!(debug.enableCh[ch].v1 && music.channel[ch].enable)) continue;
      auto sheetId = music.channel[ch].sheetId;
      auto noteId = noteBuffer[ch].noteIdx;
      auto& sheet = aram.musicsheet[sheetId];
      auto& note = sheet.note[noteId];
      if (!note.enable) continue;
      int timeInNote = (time - (note.start * music.noteFrameLength * HW_MUSIC_FREQ_PER_FRAME));
      auto instId = note.instrumentId;
      auto& inst = aram.instrument[instId];
      FLOAT dir = note.direction + music.masterDirection;
      if (dir < -16) dir = -16;
      if (dir > 16) dir = 16;
      FLOAT spvf = 0;
      if (inst.algorithm < arraysizeof(algorithms)) {
        spvf = algorithms[inst.algorithm](aram, noteBuffer[ch].note, timeInNote);
      }
      FLOAT dirlen = HW_MUSICNOTE_DIRECTION_LEN;
      auto& op = aram.soundOp[inst.soundOpId[0]];
      // if (ch==0) fmt::print("spvf={}\n", spvf);
      vfL += spvf * (dirlen-dir)/(dirlen*2.0);
      vfR += spvf * (dir+dirlen)/(dirlen*2.0);
    }
    FLOAT normalize = music.masterVolume;
    musicBuffer[timeInFrame*2+0] = (int16_t)(vfL * normalize);  // Left-ch
    musicBuffer[timeInFrame*2+1] = (int16_t)(vfR * normalize);  // Right-ch
  }
  frameCount++;
  if (music.noteFrameLength > 0) {
    noteCount = frameCount / music.noteFrameLength;
  }
  for (int ch=0; ch<8; ch++) {
    if (!music.channel[ch].enable) continue;
    auto sheetId = music.channel[ch].sheetId;
    auto noteId = noteBuffer[ch].noteIdx;
    auto& sheet = aram.musicsheet[sheetId];
    auto& nextNote = sheet.note[noteId + 1];
    if (noteCount == nextNote.start) {
      noteBuffer[ch].noteIdx++;
      noteBuffer[ch].note = nextNote;
    }
  }
  if (noteCount == music.endNote) {
    frameCount = 0;
    noteCount = 0;
    if (!music.flag.loop) {
      music.flag.enable = false;
    }
  }
}
