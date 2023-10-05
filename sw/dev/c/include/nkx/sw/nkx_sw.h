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

void setPalette(uint8_t palNo, uint32_t offset, uint32_t* palData, uint32_t size);
void setTile(uint8_t tileNo, uint32_t offset, uint8_t* tileData, uint32_t size);
void setTilemap(uint8_t mapNo, uint32_t offset, uint8_t* mapData, uint32_t size);

void loadAsciiFontPalette(uint8_t palNo, uint32_t* palData, uint32_t size);
void loadAsciiFontTile(uint8_t tileNo, uint8_t* tileData, uint32_t size);
void setAsciiFontBG(uint8_t bgNo, uint8_t palNo, uint8_t tileNo, uint8_t tilemapNo);
void putsBG(uint8_t bgNo, uint16_t x, uint16_t y, const char* str, uint8_t n);

}; /* namespace nkx*/

