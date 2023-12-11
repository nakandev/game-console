#pragma once
#include <cstdint>
#include <initializer_list>

enum {
  HWREG_SYSROM_BASEADDR       = 0x0000'0000,    HWREG_SYSROM_SIZE       = 16*1024,
  HWREG_FASTWORKRAM_BASEADDR  = 0x0100'0000,    HWREG_FASTWORKRAM_SIZE  = 32*1024,
  HWREG_SLOWWORKRAM_BASEADDR  = 0x0200'0000,    HWREG_SLOWWORKRAM_SIZE  = 1*1024+1024,
  HWREG_WORKRAM_END           = 0x0300'0000,    // HWREG_WORKRAM_END       = 0,
  HWREG_IORAM_BASEADDR        = 0x0300'0000,    HWREG_IORAM_SIZE        = 256*1024,  // fix to 1*1024
  HWREG_VRAM_BASEADDR         = 0x0400'0000,    HWREG_VRAM_SIZE         = 512*1024,
  // HWREG_PALRAM_BASEADDR      = 0x0500'0000,  // HWREG_PALRAM_SIZE       = 2*1024,
  HWREG_TILERAM_BASEADDR      = 0x0600'0000,    HWREG_TILERAM_SIZE      = 8*1024*1024,  // fix to 1*1024*1024
  HWREG_ARAM_BASEADDR         = 0x0700'0000,    HWREG_ARAM_SIZE         = 1*1024,
  // HWREG_APALRAM_BASEADDR     = 0x0800'0000,  // HWREG_APALRAM_SIZE      = 32*1024,
  HWREG_INSTRAM_BASEADDR      = 0x0900'0000,    HWREG_INSTRAM_SIZE     = 64*1024,
  HWREG_SAVERAM_BASEADDR      = 0x0E00'0000,    HWREG_SAVERAM_SIZE      = 1*1024,  // Max 512*1024*1024
  HWREG_PROGRAM_BASEADDR      = 0x8000'0000,    HWREG_PROGRAM_SIZE      = 1*1024*1024,  // Max 2*1024*1024*1024
};

/* -------- IO -------- */
enum {
  HWREG_IO_SCREEN_W_ADDR = HWREG_IORAM_BASEADDR + 0x0000,
  HWREG_IO_SCREEN_H_ADDR = HWREG_IORAM_BASEADDR + 0x0002,
  HWREG_IO_SCANLINE_ADDR = HWREG_IORAM_BASEADDR + 0x0010,
  HWREG_IO_AUDIO_XX_ADDR = HWREG_IORAM_BASEADDR + 0x0100,
  HWREG_IO_PAD0_ADDR = HWREG_IORAM_BASEADDR + 0x0200,
  HWREG_IO_PAD1_ADDR = HWREG_IORAM_BASEADDR + 0x0210,
  HWREG_IO_KEYBOARD_ADDR = HWREG_IORAM_BASEADDR + 0x0220,
  HWREG_IO_TOUCH_ADDR = HWREG_IORAM_BASEADDR + 0x0230,
  HWREG_IO_DMA0_ADDR = HWREG_IORAM_BASEADDR + 0x0300,
  HWREG_IO_TIMER0_ADDR = HWREG_IORAM_BASEADDR + 0x0400,
  HWREG_IO_SERIAL_ADDR = HWREG_IORAM_BASEADDR + 0x0500,
  HWREG_IO_INT_ENABLE_ADDR = HWREG_IORAM_BASEADDR + 0x0600,
  HWREG_IO_INT_STATUS_ADDR = HWREG_IORAM_BASEADDR + 0x0604,
  HWREG_IO_INT_VECTOR_ADDR = HWREG_IORAM_BASEADDR + 0x0608,
};

// IO Video
struct HwIoVideo {
  uint32_t screenW;
  uint32_t screenH;
  uint32_t scanline;
};

// IO Audio
struct HwIoAudio {
  uint32_t audio;
};

// IO Input
enum {
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
};

union HwPad {
  struct {
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
  uint32_t val32;
};

struct HwKeyboard {
  uint8_t code;
  uint8_t modifier;
  uint8_t on : 1;
  uint8_t _reserved0 : 7;
  uint8_t _reserved1;
};

struct HwTouch {
  uint16_t x;
  uint16_t y;
};

struct HwIoInput {
  HwPad pad[2];
  HwKeyboard keyboard;
  HwTouch touch;
};

// IO DMA
enum {
  HW_IO_DMA_SIZE_8BIT = 0,
  HW_IO_DMA_SIZE_16BIT = 1,
  HW_IO_DMA_SIZE_32BIT = 2,
  HW_IO_DMA_MODE_BURST = 0,
  HW_IO_DMA_MODE_CYCLESTEAL = 1,
  HW_IO_DMA_INCREMENT_INC = 0,
  HW_IO_DMA_INCREMENT_DEC = 1,
  HW_IO_DMA_INCREMENT_STAY = 2,
  HW_IO_DMA_TRIGGER_IMMEDIATELY = 0,
};

struct HwDma {
  uint32_t src;
  uint32_t dst;
  union {
    struct {
      uint32_t size  : 2;
      uint32_t count : 14;
      uint32_t enable : 1;
      uint32_t repeat : 1;
      uint32_t interrupt : 1;
      uint32_t mode : 1;
      uint32_t srcIncrement : 2;
      uint32_t dstIncrement : 2;
      uint32_t trigger : 4;
      uint32_t _reserved : 4;
    };
    uint32_t attribute;
  };
  uint32_t _reserved2;
};

struct HwIoDma {
  HwDma dma[4];
};

// IO Timer
struct HwIoTimer {
  uint32_t timer[4];
};

// IO Serial
struct HwIoSerial {
  uint32_t serial;
};

// IO Interrupt
enum {
  // INT_ENABLE_BIT / STATUS_BIT
  HW_IO_INT_HBLANK = 0,
  HW_IO_INT_VBLANK,
  HW_IO_INT_PAD0,
  HW_IO_INT_PAD1,
  HW_IO_INT_KEYBOARD,
  HW_IO_INT_TOUCH,
  HW_IO_INT_DMA0,
  HW_IO_INT_DMA1,
  HW_IO_INT_DMA2,
  HW_IO_INT_DMA3,
  HW_IO_INT_TIMER0,
  HW_IO_INT_TIMER1,
  HW_IO_INT_TIMER2,
  HW_IO_INT_TIMER3,
  HW_IO_INT_SAVERAM,
  HW_IO_INT_SERIAL,
};

struct HwIoInterrupt {
  uint32_t enable;
  uint32_t status;
  uint32_t vector;
};

// IORAM
struct HwIoRam {
  __attribute__((aligned(0x100))) HwIoVideo video;
  __attribute__((aligned(0x100))) HwIoAudio audio;
  __attribute__((aligned(0x100))) HwIoInput input;
  __attribute__((aligned(0x100))) HwIoDma dma;
  __attribute__((aligned(0x100))) HwIoTimer timer;
  __attribute__((aligned(0x100))) HwIoSerial serial;
  __attribute__((aligned(0x100))) HwIoInterrupt intr;
};

/* -------- Video -------- */

// Color
enum {
  HWCOLOR_BYTESIZE = 4,
};

union HwColor {
  struct {
    uint8_t a;
    uint8_t b;
    uint8_t g;
    uint8_t r;
  };
  uint32_t data;
};

// Palette
enum {
  HWPALETTE_COLORNUM = 256,
  HWPALETTE_BYTESIZE = 1,
  HWREG_PALETTE_BASEADDR   = HWREG_TILERAM_BASEADDR + 0x0000'0000,
  HWREG_PALETTE_BYTESIZE   = 0x0400,  // 4 * 256
  HWREG_PALETTE0_ADDR    = HWREG_PALETTE_BASEADDR + HWREG_PALETTE_BYTESIZE * 0,
  HWREG_PALETTE1_ADDR    = HWREG_PALETTE_BASEADDR + HWREG_PALETTE_BYTESIZE * 1,
  HWREG_PALETTE2_ADDR    = HWREG_PALETTE_BASEADDR + HWREG_PALETTE_BYTESIZE * 2,
  HWREG_PALETTE3_ADDR    = HWREG_PALETTE_BASEADDR + HWREG_PALETTE_BYTESIZE * 3,
};

union HwPalette {
  uint32_t data[HWPALETTE_COLORNUM];
  HwColor color[HWPALETTE_COLORNUM];
};

// Tile
enum {
  HWTILE_W = 8,
  HWTILE_H = 8,
  HWTILE_BYTESIZE = HWTILE_W * HWTILE_H * HWPALETTE_BYTESIZE,  // 64
  HWREG_TILE_BASEADDR      = HWREG_TILERAM_BASEADDR + 0x0010'0000,
  HWREG_TILE_BYTESIZE    = 0x00010000,
  HWREG_TILE0_ADDR    = HWREG_TILE_BASEADDR + HWREG_TILE_BYTESIZE * 0,
  HWREG_TILE1_ADDR    = HWREG_TILE_BASEADDR + HWREG_TILE_BYTESIZE * 1,
  HWREG_TILE2_ADDR    = HWREG_TILE_BASEADDR + HWREG_TILE_BYTESIZE * 2,
  HWREG_TILE3_ADDR    = HWREG_TILE_BASEADDR + HWREG_TILE_BYTESIZE * 3,
};

struct HwTile {
  uint8_t data[HWTILE_H][HWTILE_W];  // paletteIdx
};

// Tilemap
enum {
  HWBG_TILENUM = 64 * 64,  // 4096
  HWREG_TILEMAP_BASEADDR   = HWREG_TILERAM_BASEADDR + 0x0020'0000,
  HWREG_TILEMAP_BYTESIZE   = 0x2000,
  HWREG_TILEMAP0_ADDR      = HWREG_TILEMAP_BASEADDR + HWREG_TILEMAP_BYTESIZE * 0,
  HWREG_TILEMAP1_ADDR      = HWREG_TILEMAP_BASEADDR + HWREG_TILEMAP_BYTESIZE * 1,
  HWREG_TILEMAP2_ADDR      = HWREG_TILEMAP_BASEADDR + HWREG_TILEMAP_BYTESIZE * 2,
  HWREG_TILEMAP3_ADDR      = HWREG_TILEMAP_BASEADDR + HWREG_TILEMAP_BYTESIZE * 3,
};

struct HwTilemap {
  uint16_t tileIdx[HWBG_TILENUM];
};

// BG
enum {
  HW_SCREEN_W = 320,
  HW_SCREEN_H = 240,
  HW_SCREEN_HBLANK = 80,
  HW_SCREEN_VBLANK = 80,
  HW_SCREEN_XTILE = HW_SCREEN_W / HWTILE_W,
  HW_SCREEN_YTILE = HW_SCREEN_H / HWTILE_H,
  HW_TILEMAP_W = 512,
  HW_TILEMAP_H = 512,
  HW_TILEMAP_XTILE = HW_TILEMAP_W / HWTILE_W,
  HW_TILEMAP_YTILE = HW_TILEMAP_H / HWTILE_H,
  HWREG_BG_BASEADDR        = HWREG_TILERAM_BASEADDR + 0x0030'0000,
  HWREG_BG_BYTESIZE        = 0x100,
  HWREG_BG0_ADDR      = HWREG_BG_BASEADDR + HWREG_BG_BYTESIZE * 0,
  HWREG_BG1_ADDR      = HWREG_BG_BASEADDR + HWREG_BG_BYTESIZE * 1,
  HWREG_BG2_ADDR      = HWREG_BG_BASEADDR + HWREG_BG_BYTESIZE * 2,
  HWREG_BG3_ADDR      = HWREG_BG_BASEADDR + HWREG_BG_BYTESIZE * 3,
  HWBG_TILE_MODE = 0,
  HWBG_PIXEL_MODE = 1,
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
      uint8_t _reserved : 1;
    };
    uint8_t data;
  } flag;
  int16_t x;
  int16_t y;
  HwMatrix2d affineInv;
};

// SP
enum {
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
};

struct HwSprite {
  union {
    struct {
      uint8_t enable    : 1;
      uint8_t vflip     : 1;
      uint8_t hflip     : 1;
      uint8_t layer     : 2;
      uint8_t affineEnable : 1;
      uint8_t _reserved : 2;
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

// TILERAM
struct HwTileRam {
  __attribute__((aligned(0x10'0000))) HwPalette palette[0x10'0000/sizeof(HwPalette)];
  __attribute__((aligned(0x10'0000))) HwTile tile[16][0x10'0000/sizeof(HwTile)/16];
  __attribute__((aligned(0x10'0000))) HwTilemap tilemap[0x10'0000/sizeof(HwTilemap)];
  __attribute__((aligned(0x10'0000))) HwBG bg[0x10'0000/sizeof(HwBG)];
  __attribute__((aligned(0x10'0000))) HwSP sp[0x10'0000/sizeof(HwSP)];
};

// VRAM
struct HwVRam {
  HwColor pixel[HW_SCREEN_W * HW_SCREEN_H];
};

/* -------- Audio -------- */
enum {
  HWREG_INSTRUMENT_BASEADDR = HWREG_ARAM_BASEADDR + 0x0000'0000,
  HW_SOUNDOP_SIN = 0,
  HW_SOUNDOP_SQUARE = 1,
  HW_SOUNDOP_SAWTOOTH = 2,
  HW_SOUNDOP_NOISE = 3,
  HW_SPECTRUM_MIN = 0,
  HW_SPECTRUM_MAX = 128,
  HW_INSTRUMENT_TIMESLICE = 16,
  HW_INSTRUMENT_NUM = 64,
  HW_MUSICNOTE_NUM = 256,
  HW_MUSICNOTE_DIRECTION_LEN = 16,
  HW_MUSICSHEET_NUM = 16,  // 8-ch * 2-slot
  HW_MUSIC_CHANNEL_NUM = 8,
  // HW_MUSIC_FREQUENCY = 32768,
  HW_MUSIC_FREQUENCY = 30720,
  HW_MUSIC_FREQ_PER_FRAME = 512,
  HW_MUSIC_DEFAULT_NOTE_FRAMELEN = 60,
};

struct HwSoundOp {
  uint32_t func : 2;
  uint32_t arg  : 5;
  uint32_t A : 6;
  uint32_t D : 6;
  uint32_t S : 6;
  uint32_t R : 7;
  // uint32_t _reserved0 : 1;
  int16_t amp      : 10;  // 1bit:signed, 5bit:upper, 4bit:lower.
  int16_t _reserved1 : 6;
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
      uint8_t _reserved : 6;
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
  uint16_t _reserved21to22;
  uint8_t  _reserved23;
  int8_t  masterDirection;
  uint32_t masterVolume   : 16;
  uint32_t _reserved24to26 :  8;
  uint32_t noteCount      :  8;  // auto update
  uint32_t freqCount;  // auto update
};

struct HwARam {
  __attribute__((aligned(0x00'4000))) HwSoundOp soundOp[0x00'1000/sizeof(HwSoundOp)];  // 128*4*8 (soundOpNum*4module*struct)
  __attribute__((aligned(0x00'4000))) HwInstrument instrument[0x00'0800/sizeof(HwInstrument)];  // 256*8=0x800 (instNum*struct)
  __attribute__((aligned(0x01'0000))) HwMusicsheet musicsheet[0x01'0000/sizeof(HwMusicsheet)];  // 32*256*8 (sheets*notes*struct)
  __attribute__((aligned(0x00'1000))) HwMusic music[0x00'1000/sizeof(HwMusic)];  // 8*32 (musics*struct)
};
