#pragma once
#include <cstdint>
#include <initializer_list>

enum {
  HWREG_SYSROM_BASEADDR       = 0x0000'0000,
  HWREG_FASTWORKRAM_BASEADDR  = 0x0100'0000,
  HWREG_SLOWWORKRAM_BASEADDR  = 0x0200'0000,
  HWREG_WORKRAM_END           = 0x0300'0000,
  HWREG_IORAM_BASEADDR        = 0x0300'0000,
  HWREG_VRAM_BASEADDR         = 0x0400'0000,
  HWREG_TILERAM_BASEADDR      = 0x0600'0000,
  HWREG_ARAM_BASEADDR         = 0x0700'0000,
  HWREG_PROGRAM_BASEADDR      = 0x0800'0000,
  HWREG_SAVERAM_BASEADDR      = 0x0E00'0000,

  HW_IO_PAD0 = HWREG_IORAM_BASEADDR + 0x0000,
  HW_IO_PAD1 = HWREG_IORAM_BASEADDR + 0x0010,
  HW_IO_PAD_ADDR = HWREG_IORAM_BASEADDR + 0x0000,
  HW_IO_EXTINT_ENABLE_ADDR = HWREG_IORAM_BASEADDR + 0x0100,
  HW_IO_EXTINT_STATUS_ADDR = HWREG_IORAM_BASEADDR + 0x0200,
  HW_IO_EXTINT_VECTOR_ADDR = HWREG_IORAM_BASEADDR + 0x1000,
  // INT_EXT_CAUSE_BIT / STATUS_BIT
  HW_IO_EXTINT_HBLANK = 16,
  HW_IO_EXTINT_VBLANK,
  HW_IO_EXTINT_PAD,
  HW_IO_EXTINT_DMA0,
  HW_IO_EXTINT_DMA1,
  HW_IO_EXTINT_DMA2,
  HW_IO_EXTINT_DMA3,
  HW_IO_EXTINT_SERIAL,
  HW_IO_EXTINT_24,
  HW_IO_EXTINT_25,
  HW_IO_EXTINT_26,
  HW_IO_EXTINT_27,
  HW_IO_EXTINT_28,
  HW_IO_EXTINT_29,
  HW_IO_EXTINT_30,
  // HW_IO_EXTINT_31,  // mstatus:bit[31] is reserved.

  HW_PAD_A = 0,
  HW_PAD_B,
  HW_PAD_C,
  HW_PAD_D,
  HW_PAD_L,
  HW_PAD_R,
  HW_PAD_S,
  HW_PAD_T,
  HW_PAD_RESERVED0,
  HW_PAD_RESERVED1,
  HW_PAD_RESERVED2,
  HW_PAD_RESERVED3,
  HW_PAD_UP,
  HW_PAD_DOWN,
  HW_PAD_LEFT,
  HW_PAD_RIGHT,

  HWCOLOR_BYTESIZE = 4,

  HWPALETTE_COLORNUM = 256,
  HWPALETTE_BYTESIZE = 1,
  HWREG_PALETTE_BASEADDR   = HWREG_TILERAM_BASEADDR + 0x0000'0000,
  HWREG_PALETTE_BYTESIZE   = 0x0400,
  HWREG_PALETTE0_ADDR    = HWREG_PALETTE_BASEADDR + HWREG_PALETTE_BYTESIZE * 0,
  HWREG_PALETTE1_ADDR    = HWREG_PALETTE_BASEADDR + HWREG_PALETTE_BYTESIZE * 1,
  HWREG_PALETTE2_ADDR    = HWREG_PALETTE_BASEADDR + HWREG_PALETTE_BYTESIZE * 2,
  HWREG_PALETTE3_ADDR    = HWREG_PALETTE_BASEADDR + HWREG_PALETTE_BYTESIZE * 3,

  HWTILE_W = 8,
  HWTILE_H = 8,
  HWTILE_BYTESIZE = HWTILE_W * HWTILE_H * HWPALETTE_BYTESIZE,  // 64
  HWREG_TILE_BASEADDR      = HWREG_TILERAM_BASEADDR + 0x0010'0000,
  HWREG_TILE_BYTESIZE    = 0x00010000,
  HWREG_TILE0_ADDR    = HWREG_TILE_BASEADDR + HWREG_TILE_BYTESIZE * 0,
  HWREG_TILE1_ADDR    = HWREG_TILE_BASEADDR + HWREG_TILE_BYTESIZE * 1,
  HWREG_TILE2_ADDR    = HWREG_TILE_BASEADDR + HWREG_TILE_BYTESIZE * 2,
  HWREG_TILE3_ADDR    = HWREG_TILE_BASEADDR + HWREG_TILE_BYTESIZE * 3,

  HWBG_TILENUM = 64 * 64,  // 4096
  HWREG_TILEMAP_BASEADDR   = HWREG_TILERAM_BASEADDR + 0x0020'0000,
  HWREG_TILEMAP_BYTESIZE   = 0x2000,
  HWREG_TILEMAP0_ADDR      = HWREG_TILEMAP_BASEADDR + HWREG_TILEMAP_BYTESIZE * 0,
  HWREG_TILEMAP1_ADDR      = HWREG_TILEMAP_BASEADDR + HWREG_TILEMAP_BYTESIZE * 1,
  HWREG_TILEMAP2_ADDR      = HWREG_TILEMAP_BASEADDR + HWREG_TILEMAP_BYTESIZE * 2,
  HWREG_TILEMAP3_ADDR      = HWREG_TILEMAP_BASEADDR + HWREG_TILEMAP_BYTESIZE * 3,

  HW_SCREEN_W = 320,
  HW_SCREEN_H = 240,
  HW_SCREEN_XTILE = 40,
  HW_SCREEN_YTILE = 30,
  HW_TILEMAP_W = 512,
  HW_TILEMAP_H = 512,
  HW_TILEMAP_XTILE = 64,
  HW_TILEMAP_YTILE = 64,
  HWREG_BG_BASEADDR        = HWREG_TILERAM_BASEADDR + 0x0030'0000,
  HWREG_BG_BYTESIZE        = 0x100,
  HWREG_BG0_ADDR      = HWREG_BG_BASEADDR + HWREG_BG_BYTESIZE * 0,
  HWREG_BG1_ADDR      = HWREG_BG_BASEADDR + HWREG_BG_BYTESIZE * 1,
  HWREG_BG2_ADDR      = HWREG_BG_BASEADDR + HWREG_BG_BYTESIZE * 2,
  HWREG_BG3_ADDR      = HWREG_BG_BASEADDR + HWREG_BG_BYTESIZE * 3,
  HWBG_TILE_MODE = 0,
  HWBG_PIXEL_MODE = 1,

  HWREG_SPRITE_BASEADDR    = HWREG_TILERAM_BASEADDR + 0x0040'0000,
  HWREG_SPRITE_BYTESIZE    = 0x1000,
  HWREG_SP0_ADDR    = HWREG_SPRITE_BASEADDR + HWREG_SPRITE_BYTESIZE * 0,
  HW_SPRITE_NUM = 256,
  HW_SPRITE_FLAG_ENABLE_BIT = (1<<0),
  HW_SPRITE_FLAG_FLIPV_BIT = (1<<1),  // vertical flip
  HW_SPRITE_FLAG_FLIPH_BIT = (1<<2),  // horizontal flip
  HW_SPRITE_TILESIZE_8x8   = 0,
  HW_SPRITE_TILESIZE_16x16 = 1,
  HW_SPRITE_TILESIZE_32x32 = 2,
  HW_SPRITE_TILESIZE_64x64 = 3,

  HW_SCREEN_HBLANK = 80,
  HW_SCREEN_VBLANK = 80,

  HWREG_INSTRUMENT_BASEADDR = HWREG_ARAM_BASEADDR + 0x0000'0000,
  HW_SOUNDOP_SIN = 0,
  HW_SOUNDOP_SQUARE = 1,
  HW_SOUNDOP_SAWTOOTH = 2,
  HW_SOUNDOP_NOISE = 3,
  HW_SPECTRUM_MIN = 0,
  HW_SPECTRUM_MAX = 128,
  // HW_SPECTRUM_NUM = 16,
  HW_INSTRUMENT_TIMESLICE = 16,
  HW_INSTRUMENT_NUM = 64,
  HW_MUSICNOTE_NUM = 256,
  HW_MUSICNOTE_DIRECTION_LEN = 16,
  HW_MUSICSHEET_NUM = 16,  // 8-ch * 2-slot
  HW_MUSIC_CHANNEL_NUM = 8,
  // HW_MUSIC_FREQUENCY = 32768,
  // HW_MUSIC_FREQ_PER_FRAME = 512,
  // HW_MUSIC_DEFAULT_NOTE_FRAME = 64,
  HW_MUSIC_FREQUENCY = 30720,
  HW_MUSIC_FREQ_PER_FRAME = 512,
  HW_MUSIC_DEFAULT_NOTE_FRAMELEN = 60,
  // HW_MUSIC_DEFAULT_NOTE_FRAMELEN = 64,
};

struct HwPad {
  struct {
    uint16_t A : 1;
    uint16_t B : 1;
    uint16_t C : 1;
    uint16_t D : 1;
    uint16_t L : 1;
    uint16_t R : 1;
    uint16_t S : 1;
    uint16_t T : 1;
    uint16_t _reserved0 : 1;
    uint16_t _reserved1 : 1;
    uint16_t _reserved2 : 1;
    uint16_t _reserved3 : 1;
    uint16_t UP : 1;
    uint16_t DOWN : 1;
    uint16_t LEFT : 1;
    uint16_t RIGHT : 1;
  };
  uint8_t AnalogX;
  uint8_t AnalogY;
};

union HwColor {
  uint32_t data;
  struct {
    uint8_t a;
    uint8_t b;
    uint8_t g;
    uint8_t r;
  };
};

union HwPalette {
  uint32_t data[HWPALETTE_COLORNUM];
  HwColor color[HWPALETTE_COLORNUM];
};

struct HwTile {
  uint8_t data[HWTILE_H][HWTILE_W];  // paletteIdx
};

struct HwTilemap {
  uint16_t tileIdx[HWBG_TILENUM];
};

struct HwMatrix2d {
  int16_t a;
  int16_t b;
  int16_t c;
  int16_t d;
  int16_t x;
  int16_t y;
};

struct HwBG {
  uint8_t paletteNo;
  uint8_t tileNo;
  uint8_t tilemapNo;
  union {
    struct {
      uint8_t enable    : 1;
      uint8_t mode      : 1;
      uint8_t vflip     : 1;
      uint8_t hflip     : 1;
      uint8_t layer     : 2;
      uint8_t affineEnable : 1;
      uint8_t __padding : 1;
    };
    uint8_t data;
  } flag;
  int16_t x;
  int16_t y;
  HwMatrix2d affineInv;
};

struct HwSprite {
  union {
    struct {
      uint8_t enable    : 1;
      uint8_t vflip     : 1;
      uint8_t hflip     : 1;
      uint8_t layer     : 2;
      uint8_t affineEnable : 1;
      uint8_t __padding : 2;
    };
    uint8_t data;
  } flag;
  uint8_t paletteNo : 4;
  uint8_t tileNo    : 4;
  uint8_t tileIdx;
  uint8_t tileSize;
  int16_t x;
  int16_t y;
  HwMatrix2d affineInv;
};

struct HwSP {
  HwSprite sprite[HW_SPRITE_NUM];
};

struct HwTileRam {
  __attribute__((aligned(0x10'0000))) HwPalette palette[0x10'0000/sizeof(HwPalette)];
  __attribute__((aligned(0x10'0000))) HwTile tile[16][0x10'0000/sizeof(HwTile)/16];
  __attribute__((aligned(0x10'0000))) HwTilemap tilemap[0x10'0000/sizeof(HwTilemap)];
  __attribute__((aligned(0x10'0000))) HwBG bg[0x10'0000/sizeof(HwBG)];
  __attribute__((aligned(0x10'0000))) HwSP sp[0x10'0000/sizeof(HwSP)];
};

/* Audio */
struct HwSoundOp {
  uint32_t func : 2;
  uint32_t arg  : 5;
  uint32_t A : 6;
  uint32_t D : 6;
  uint32_t S : 6;
  uint32_t R : 7;
  // uint32_t padding0 : 1;
  int16_t amp      : 10;  // 1bit:signed, 5bit:upper, 4bit:lower.
  int16_t padding1 : 6;
  uint8_t ratio;
  uint8_t detune;
};

struct HwInstrument {  // Tile
  uint8_t soundOpId[4];
  uint8_t algorithm;
  uint8_t dummy[3];
};

struct HwMusicNote {
  uint8_t enable       : 1;
  uint8_t instrumentId : 7;
  uint8_t length  : 4;
  uint8_t channel : 4;
  int8_t direction;
  int8_t scale;
  uint32_t start;
};

struct HwMusicsheet {  // Tilemap
  HwMusicNote note[HW_MUSICNOTE_NUM];
};

struct HwMusic {  // BG
  union {
    struct {
      uint8_t enable   : 1;
      uint8_t loop     : 1;
      uint8_t _padding : 6;
    };
    uint8_t data;
  } flag;
  uint8_t loopNote;
  uint8_t endNote;
  uint8_t noteFrameLength;  // bps=(60/this)*60
  struct {
    uint8_t enable  : 1;
    uint8_t sheetId : 7;
  } channel[HW_MUSIC_CHANNEL_NUM];
  uint16_t _padding21to22;
  uint8_t  _padding23;
  int8_t  masterDirection;
  uint32_t masterVolume   : 16;
  uint32_t _padding24to26 :  8;
  uint32_t noteCount      :  8;  // auto update
  uint32_t freqCount;  // auto update
};

struct HwARam {
  __attribute__((aligned(0x00'4000))) HwSoundOp soundOp[0x00'1000/sizeof(HwSoundOp)];  // 128*4*8 (soundOpNum*4module*struct)
  __attribute__((aligned(0x00'4000))) HwInstrument instrument[0x00'0800/sizeof(HwInstrument)];  // 256*8=0x800 (instNum*struct)
  __attribute__((aligned(0x01'0000))) HwMusicsheet musicsheet[0x01'0000/sizeof(HwMusicsheet)];  // 32*256*8 (sheets*notes*struct)
  __attribute__((aligned(0x00'1000))) HwMusic music[0x00'1000/sizeof(HwMusic)];  // 8*32 (musics*struct)
};
