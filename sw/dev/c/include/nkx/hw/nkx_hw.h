#include <stdint.h>
#include <memorymap.h>

#define arraysizeof(x) (sizeof(x)/sizeof(x[0]))

namespace nkx {

using HwColor   = ::HwColor;
using HwPalette = ::HwPalette;
using HwTile    = ::HwTile;
using HwTilemap = ::HwTilemap;
using HwBG      = ::HwBG;
using HwSprite  = ::HwSprite;
using HwSP      = ::HwSP;

class Hw {
  public:
  HwPalette* palette[8];
  HwTile* tile[16];
  HwTilemap* tilemap[16];
  HwBG* bg[4];
  HwSP* sp[1];

  Hw();
  ~Hw();
};

extern volatile Hw hw;

}; /* namespace nkx*/

