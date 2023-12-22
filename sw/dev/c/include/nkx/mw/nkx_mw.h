#pragma once
namespace nkx {

/* Common */
struct Vec2 {
  union {
    struct {
      int32_t x;
      int32_t y;
    };
    struct {
      int32_t w;
      int32_t h;
    };
    int32_t v[2];
  };
};

struct Rect2 {
  union {
    struct {
      int32_t x;
      int32_t y;
      int32_t w;
      int32_t h;
    };
    int32_t v[4];
  };
};

__attribute__((noinline)) void srand(int32_t seed);
__attribute__((noinline)) int32_t rand();
uint8_t strlen(const char* str, uint8_t n);

/* IO */
void getPadStatus(uint8_t padNo);
void isOnButton(uint8_t padNo);
void isOffButton(uint8_t padNo);
void isPushButton(uint8_t padNo);
void isReleaseButton(uint8_t padNo);

/* Video */
void setPalette(uint8_t palBank, uint8_t palNo, uint32_t offset, const uint32_t* palData, uint32_t size);
void setTile(uint8_t tileNo, uint32_t offset, const uint8_t* tileData, uint32_t size);
void setTilemap(uint8_t mapNo, uint32_t offset, const uint16_t* mapData, uint32_t size);
// void setTilemap2D(uint8_t mapNo, const Vec2 mapSize, const Vec2 mapOffset, const uint16_t* mapData, const Vec2 dataSize, const Rect2 dataRect);
void setTilemap2D(
  uint8_t mapNo,
  uint32_t mapSizeW, uint32_t mapSizeH,
  uint32_t mapOffsetX, uint32_t mapOffsetY,
  const uint16_t* mapData,
  uint32_t dataSizeW, uint32_t dataSizeH,
  uint32_t dataRectX, uint32_t dataRectY, uint32_t dataRectW, uint32_t dataRectH
);
void setTilemap2DLambda(
  uint8_t mapNo,
  uint32_t mapSizeW, uint32_t mapSizeH,
  uint32_t mapOffsetX, uint32_t mapOffsetY,
  uint16_t (*mapData)(int32_t, int32_t),
  uint32_t dataSizeW, uint32_t dataSizeH,
  uint32_t dataRectX, uint32_t dataRectY, uint32_t dataRectW, uint32_t dataRectH
);
// void setSprite(uint8_t spNo, uint32_t offset, uint8_t* spData, uint32_t size);
// void setBG(uint8_t bgNo, uint32_t offset, uint8_t* bgData, uint32_t size);

void loadAsciiFontPalette(uint8_t palNo, uint32_t* palData, uint32_t size);
void loadAsciiFontTile(uint8_t tileNo, uint8_t* tileData, uint32_t size);
void setAsciiFontBG(uint8_t bgNo, uint8_t palNo, uint8_t tileNo, uint8_t tilemapNo);
void putsBG(uint8_t bgNo, uint16_t x, uint16_t y, const char* str, uint8_t n);

/* Audio */
void setSoundOp(uint8_t sopNo, uint32_t offset, uint8_t* sopData, uint32_t size);
void setInstrument(uint8_t instrNo, uint32_t offset, uint8_t* instrData, uint32_t size);
void setMusicsheet(uint8_t sheetNo, uint32_t offset, uint8_t* sheetData, uint32_t size);
// void setMusic(uint8_t musicNo, uint32_t offset, uint8_t* musicData, uint32_t size);
}; /* namespace nkx*/

