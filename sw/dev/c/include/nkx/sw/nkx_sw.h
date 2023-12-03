namespace nkx {

/* GameEvent */
/* GameObject */
class GameObject {
  // Sprite* sprite;
  // Collision* collistion;
  // Position* position;
  // EventHandler* eventHandlers;
  // connectEvent(event, handler);
  // spawn();
  // update();
  // destroy();
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
void setPalette(uint8_t palNo, uint32_t offset, uint32_t* palData, uint32_t size);
void setTile(uint8_t tileNo, uint32_t offset, uint8_t* tileData, uint32_t size);
void setTilemap(uint8_t mapNo, uint32_t offset, uint8_t* mapData, uint32_t size);
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

