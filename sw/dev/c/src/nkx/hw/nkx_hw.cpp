#include <nkx/hw/nkx_hw.h>

namespace nkx {

#ifdef BUILD_FOR_SIMULATOR
uint8_t HwReg[0x40000000];
#else
uint8_t* HwReg = nullptr;
#endif

Hw hw;


HwColor::HwColor()
  : r(0), g(0), b(0), a(255)
{
}

HwColor::HwColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  : r(r), g(g), b(b), a(a)
{
}

HwColor::HwColor(uint32_t data)
  : r((data>>0)&0xffu), g((data>>8)&0xffu), b((data>>16)&0xffu), a((data>>24)&0xffu)
{
}

HwColor::~HwColor()
{
}

void HwColor::set(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  this->r = r;
  this->g = g;
  this->b = b;
  this->a = a;
}

uint32_t HwColor::data()
{
  return (a<<24) | (b<<16) | (g<<8) | r;
}

Hw::Hw()
{
  palette[0] = (HwPalette*)(HwReg + HWREG_PALETTE0_ADDR);
  tile[0] = (HwTile*)(HwReg + HWREG_TILE0_ADDR);
  tilemap[0] = (HwTilemap*)(HwReg + HWREG_TILEMAP0_ADDR);
  bg[0] = (HwBG*)(HwReg + HWREG_BG0_ADDR);
}

Hw::~Hw()
{
}

}; /* namespace nkx*/

