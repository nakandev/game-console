#include <nkx/hw/nkx_hw.h>

namespace nkx {

#ifdef BUILD_FOR_SIMULATOR
uint8_t HwReg[0x40000000];
#else
uint8_t* HwReg = nullptr;
#endif

volatile Hw hw;

Hw::Hw()
{
  for (int i=0; i<arraysizeof(palette); i++)
    palette[i] = (HwPalette*)(HwReg + HWREG_PALETTE0_ADDR + sizeof(HwPalette) * i);
  for (int i=0; i<arraysizeof(tile); i++)
    tile[i] = (HwTile*)(HwReg + HWREG_TILE0_ADDR + HWREG_TILE_BYTESIZE * i);
  for (int i=0; i<arraysizeof(tilemap); i++)
    tilemap[i] = (HwTilemap*)(HwReg + HWREG_TILEMAP0_ADDR + HWREG_TILEMAP_BYTESIZE * i);
  for (int i=0; i<arraysizeof(bg); i++)
    bg[i] = (HwBG*)(HwReg + HWREG_BG0_ADDR + HWREG_BG_BYTESIZE * i);
  for (int i=0; i<arraysizeof(sp); i++)
    sp[i] = (HwSP*)(HwReg + HWREG_SP0_ADDR + HWREG_SPRITE_BYTESIZE * i);
}

Hw::~Hw()
{
}

}; /* namespace nkx*/

