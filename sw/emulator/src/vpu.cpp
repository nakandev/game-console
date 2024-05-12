#include <vpu.h>
#include <memory.h>
#include <io.h>
#include <fmt/core.h>

Vpu::Vpu(Memory& memory)
  : _currentLineNo(),
    lineBufferBg(),
    lineBufferSp(),
    screenBuffer(),
    vpuSprite(),
    memory(memory),
    vramSection("vram", HWREG_VRAM_BASEADDR   , HWREG_VRAM_SIZE),
    tileSection("tile", HWREG_TILERAM_BASEADDR, HWREG_TILERAM_SIZE),
    scanline(),
    debug()
{
  lineBufferBg.resize(4);
  for (auto& buffer: lineBufferBg)
    buffer.resize(HW_SCREEN_W);
  lineBufferSp.resize(4);
  for (auto& buffer: lineBufferSp)
    buffer.resize(HW_SCREEN_W);
  screenBuffer.resize(HW_SCREEN_W * HW_SCREEN_H);
  vpuSprite.resize(HW_SPRITE_NUM);
  debug.enableBg.resize(4);
  for (auto& enable: debug.enableBg)
    enable.v1 = true;
  debug.enableSp.v1 = true;
}

Vpu::~Vpu()
{
  for (auto& buffer: lineBufferBg)
    buffer.clear();
  for (auto& buffer: lineBufferSp)
    buffer.clear();
  lineBufferBg.clear();
  lineBufferSp.clear();
  screenBuffer.clear();
  vpuSprite.clear();
}

auto Vpu::init() -> void
{
  memory.addSection<MemorySection>(&vramSection);
  memory.addSection<MemorySection>(&tileSection);
}

auto Vpu::currentLineNo() -> int
{
  return _currentLineNo;
}

static const int widthTable[] = {8, 16, 32, 64};
static const int heightTable[] = {8, 16, 32, 64};
static auto prepareVpuSprite(HwTileRam& tileram, vector<VpuSprite>& vpusp, int y) -> int
{
  int spriteNum = 0;
  for (int i=0; i<HW_SPRITE_NUM; i++) {
    HwSprite& sp = tileram.sp[0].sprite[i];
    if (!sp.enable) continue;
    int16_t w = widthTable[sp.tileSize];
    int16_t h = heightTable[sp.tileSize];
    int16_t beginX = sp.x;
    int16_t beginY = sp.y;
    int16_t endX = sp.x + w - 1;
    int16_t endY = sp.y + h - 1;
    if (sp.affineEnable) {
      beginX -= w / 2;
      beginY -= h / 2;
      endX += w / 2;
      endY += h / 2;
    }
    if (y < beginY || endY < y) continue;
    vpusp[spriteNum].hwsp = sp;
    vpusp[spriteNum].beginY = beginY;
    vpusp[spriteNum].endY   = endY;
    vpusp[spriteNum].beginX = beginX;
    vpusp[spriteNum].endX   = endX;
    vpusp[spriteNum].idx = i;
    spriteNum++;
  }
  return spriteNum;
}

static auto color_merge(HwColor& dst, const HwColor& src) -> bool
{
  uint8_t a = src.a;
  if (a == 0) return false;
  if (a == 255) {
    dst.data = src.data;
    return true;
  } else {
    dst.r = (uint8_t)(((uint16_t)dst.r * (255 - a) + (uint16_t)src.r * a) / 256);
    dst.g = (uint8_t)(((uint16_t)dst.g * (255 - a) + (uint16_t)src.g * a) / 256);
    dst.b = (uint8_t)(((uint16_t)dst.b * (255 - a) + (uint16_t)src.b * a) / 256);
    dst.a = (dst.a + src.a) / 2;
    return false;
  }
}

static auto drawLineBGPixel(uint8_t* vram, HwBG& bg, int x, int y, vector<uint32_t>& buffer) -> void
{
  HwColor* colors = (HwColor*) vram;
  buffer[x] = colors[x + y * HW_SCREEN_W].data;
}

static auto drawLineBGTile(HwTileRam& tileram, HwBG& bg, int x, int y, vector<uint32_t>& buffer) -> void
{
  int32_t x0 = x - bg.x;
  int32_t y0 = y - bg.y;
  if (bg.affineEnable) {
    HwMatrix2d B = bg.affineInv;  // B = inv(A)
    int32_t x00 = x0 - B.x;
    int32_t y00 = y0 - B.y;
    // float div = B.a/256. * B.d/256. - B.b/256. * B.c/256.;
    // x0 = (x00 * +B.d/256. + y00 * -B.b/256.) / div + B.x;
    // y0 = (x00 * -B.c/256. + y00 * +B.a/256.) / div + B.y;
    x0 = (x00 * B.a/256. + y00 * B.b/256.) + B.x;
    y0 = (x00 * B.c/256. + y00 * B.d/256.) + B.y;
    if (x0 < 0 || x0 >= 512) return;
    if (y0 < 0 || y0 >= 512) return;
  }
  uint32_t tx0 = ((uint32_t)x0) % HW_TILEMAP_W;
  uint32_t ty0 = ((uint32_t)y0) % HW_TILEMAP_H;
  uint8_t tw = widthTable[bg.tileSize];
  uint8_t th = heightTable[bg.tileSize];
  uint16_t offset0 = (tx0 / tw) + (ty0 / tw) * (HW_TILEMAP_W / tw);
  uint8_t tilemapBank = bg.tilemapBank;
  uint16_t tilemapIdx = tileram.tilemap[tilemapBank].tileIdx[offset0].data;
  uint16_t offset1x = tx0 % tw / HW_TILE_W;
  uint16_t offset1y = ty0 % th / HW_TILE_H * (tw / HW_TILE_W);
  uint16_t tileIdx = tilemapIdx * (tw * th / HW_TILE_H / HW_TILE_W) + offset1x + offset1y;
  uint32_t tilex = ty0 % HW_TILE_H;
  uint32_t tiley = tx0 % HW_TILE_W;
  uint8_t tileBank = bg.tileBank;
  uint8_t paletteIdx = tileram.tile[tileBank][tileIdx].data[tilex][tiley];
  uint8_t paletteBank = bg.paletteInfo.bank;
  uint8_t paletteNo = bg.paletteInfo.no;
  uint8_t paletteOffset;
  if (bg.paletteInfo.mode == HW_PALETTE_MODE_256) {
    paletteOffset = paletteIdx;
  } else /* if (paletteInfo.mode == HW_PALETTE_MODE_16) */ {
    paletteOffset = paletteIdx + paletteNo * 16;
  }
  HwColor color = tileram.palette[paletteBank].color[paletteOffset];
  if (color.a > 0 && bg.paletteInfo.transparency > 0) {
    color.a = 0xFFu - (bg.paletteInfo.transparency * 0x11u);
  }
  buffer[x] = color.data;
}

static auto drawLineSprite(HwTileRam& tileram, VpuSprite& vpusp, int x, int y, vector<uint32_t>& buffer) -> void
{
  HwSprite& sp = vpusp.hwsp;
  int32_t x0 = x - sp.x;
  int32_t y0 = y - sp.y;
  if (sp.affineEnable) {
    HwMatrix2d B = sp.affineInv;  // B = inv(A)
    int32_t x00 = x0 - B.x;
    int32_t y00 = y0 - B.y;
    // float div = B.a/256. * B.d/256. - B.b/256. * B.c/256.;
    // x0 = (x00 * +B.d/256. + y00 * -B.b/256.) / div + B.x;
    // y0 = (x00 * -B.c/256. + y00 * +B.a/256.) / div + B.y;
    x0 = (x00 * B.a/256. + y00 * B.b/256.) + B.x;
    y0 = (x00 * B.c/256. + y00 * B.d/256.) + B.y;
  }
  if (x0 < 0 || x0 >= widthTable[sp.tileSize]) return;
  if (y0 < 0 || y0 >= heightTable[sp.tileSize]) return;
  uint32_t tx0 = ((uint32_t)x0) % HW_TILEMAP_W;
  uint32_t ty0 = ((uint32_t)y0) % HW_TILEMAP_H;
  uint8_t tileBank = sp.tileBank;
  uint8_t offset = (x0 / HW_TILE_W) + (y0 / HW_TILE_H) * (widthTable[sp.tileSize] / HW_TILE_W);
  uint8_t tileIdx = sp.tileIdx + offset;
  uint32_t tilex = tx0 % HW_TILE_W;
  uint32_t tiley = ty0 % HW_TILE_H;
  uint8_t paletteIdx = tileram.tile[tileBank][tileIdx].data[tiley][tilex];
  uint8_t paletteBank = sp.paletteInfo.bank;
  uint8_t paletteNo = sp.paletteInfo.no;
  uint8_t paletteOffset;
  if (sp.paletteInfo.mode == HW_PALETTE_MODE_256) {
    paletteOffset = paletteIdx;
  } else /* if (paletteInfo.mode == HW_PALETTE_MODE_16) */ {
    paletteOffset = paletteIdx + paletteNo * 16;
  }
  HwColor color = tileram.palette[paletteBank].color[paletteOffset];
  if (color.a > 0 && sp.paletteInfo.transparency > 0) {
    color.a = 0xFFu - (sp.paletteInfo.transparency * 0x11u);
  }
  HwColor bufColor = {.data = buffer[x]};
  color_merge(bufColor, color);
  buffer[x] = bufColor.data;
}

static void sortLayerIndex(int idxs[4], int layers[4])
{
  int t;
  for (int i=3; i>=0; i--) {
    for (int j=0; j<i; j++) {
      if (layers[j] > layers[j+1]) {
        t = layers[j]; layers[j] = layers[j+1]; layers[j+1] = t;
        t = idxs[j]; idxs[j] = idxs[j+1]; idxs[j+1] = t;
      }
    }
  }
}

auto Vpu::drawLine(int y) -> void
{
  HwTileRam& tileram = *(HwTileRam*)memory.section("tile").buffer();
  uint8_t* vram = (uint8_t*)memory.section("vram").buffer();
  int spnum = prepareVpuSprite(tileram, vpuSprite, y);
  for (auto& buf: lineBufferBg) {
    std::fill(buf.begin(), buf.end(), 0);
  }
  for (auto& buf: lineBufferSp) {
    std::fill(buf.begin(), buf.end(), 0);
  }
  int idxs[4] = {0, 1, 2, 3};
  int layers[4];
  for (int i=0; i<4; i++) layers[i] = tileram.bg[i].layer;
  sortLayerIndex(idxs, layers);
  for (int x=0; x<HW_SCREEN_W; x++) {
    for (int i=0; i<4; i++) {
      uint8_t layer = idxs[i];
      auto& bg = tileram.bg[layer];
      if (debug.enableBg[layer].v1 && bg.enable) {
        if (bg.mode == HW_BG_PIXEL_MODE) {
          drawLineBGPixel(vram, tileram.bg[layer], x, y, lineBufferBg[layer]);
        } else {
          drawLineBGTile(tileram, tileram.bg[layer], x, y, lineBufferBg[layer]);
        }
      }
    }
  }
  if (debug.enableSp.v1) {
    for (int x=0; x<HW_SCREEN_W; x++) {
      HwSP& hwsp = tileram.sp[0];
      for (int si=0; si<spnum; si++) {
        auto& vpusp = vpuSprite[si];
        uint8_t layer = vpusp.hwsp.layer;
        if (x < vpusp.beginX || vpusp.endX < x) continue;
        drawLineSprite(tileram, vpusp, x, y, lineBufferSp[layer]);
      }
    }
  }
  for (int x=0; x<HW_SCREEN_W; x++) {
    HwColor finalColor = {.data = 0};
    for (int layer=0; layer<4; layer++) {
      HwColor spColor = {.data=lineBufferSp[layer][x]};
      if (color_merge(finalColor, spColor)) /*break*/;
      HwColor bgColor = {.data=lineBufferBg[layer][x]};
      if (color_merge(finalColor, bgColor)) /*break*/;
    }
    finalColor.a = 255;

    screenBuffer[x + y*HW_SCREEN_W] = finalColor.data;
  }
}

static auto isInDrawAreaY(uint8_t objSize, int16_t objY, int8_t affineEnable) -> uint8_t
{
  int16_t h = heightTable[objSize];
  int16_t beginY = objY;
  int16_t endY = objY + h - 1;
  if (affineEnable) {
    beginY -= h / 2;
    endY += h / 2;
  }
  if (objY < beginY || endY < objY) false;
  return true;
}

static auto getDrawAreaX(uint8_t objSize, int16_t objX, int8_t affineEnable) -> uint32_t
{
  int16_t w = widthTable[objSize];
  int16_t beginX = objX;
  int16_t endX = objX + w - 1;
  if (affineEnable) {
    beginX -= w / 2;
    endX += w / 2;
  }
  uint32_t x0x1 = ((int32_t)beginX << 16) + (endX);
  return x0x1;
}

static auto affineTransform(int16_t x, int16_t y, int16_t objX, int16_t objY, int8_t affineEnable, HwMatrix2d& B) -> uint32_t
{
  int16_t objXa = x - objX;
  int16_t objYa = y - objY;
  if (affineEnable) {
    // HwMatrix2d B = sp.affineInv;  // B = inv(A)
    int32_t x0 = objX - B.x;
    int32_t y0 = objY - B.y;
    objXa = (x0 * B.a/256. + y0 * B.b/256.) + B.x;
    objYa = (x0 * B.c/256. + y0 * B.d/256.) + B.y;
  }
  uint32_t xaya = ((int32_t)objXa << 16) | (objYa);
  return xaya;
}

static auto getTileIdxSP(HwTileRam& tileram, uint8_t objSize, int16_t objXa, int16_t objYa, uint8_t spTileIdx) -> uint16_t
{
  uint32_t tx0 = ((uint32_t)objXa) % HW_TILEMAP_W;
  uint32_t ty0 = ((uint32_t)objYa) % HW_TILEMAP_H;
  uint8_t offset = (tx0 / HW_TILE_W) + (ty0 / HW_TILE_H) * (widthTable[objSize] / HW_TILE_W);
  uint16_t tileIdx = spTileIdx + offset;
  return tileIdx;
}

static auto getTileIdxBG(HwTileRam& tileram, uint8_t objSize, int16_t objXa, int16_t objYa, uint8_t tilemapBank) -> uint16_t
{
  uint32_t tx0 = ((uint32_t)objXa) % HW_TILEMAP_W;
  uint32_t ty0 = ((uint32_t)objYa) % HW_TILEMAP_H;
  uint8_t tw = widthTable[objSize];
  uint8_t th = heightTable[objSize];
  uint16_t offset0 = (tx0 / tw) + (ty0 / th) * (HW_TILEMAP_W / tw);
  uint16_t tilemapIdx = tileram.tilemap[tilemapBank].tileIdx[offset0].data;
  uint16_t offset1x = tx0 % tw / HW_TILE_W;
  uint16_t offset1y = ty0 % th / HW_TILE_H * (tw / HW_TILE_W);
  uint16_t tileIdx = tilemapIdx * (tw * th / HW_TILE_H / HW_TILE_W) + offset1x + offset1y;
  return tileIdx;
}

static auto getPaletteIdx(HwTileRam& tileram, int16_t objXa, int16_t objYa, uint8_t tileBank, uint16_t tileIdx) -> uint8_t
{
  uint32_t tilex = ((uint32_t)objXa) % HW_TILEMAP_W % HW_TILE_W;
  uint32_t tiley = ((uint32_t)objYa) % HW_TILEMAP_H % HW_TILE_H;
  uint8_t paletteIdx = tileram.tile[tileBank][tileIdx].data[tiley][tilex];  // Memory Access
  return paletteIdx;
}

auto colorMergeSP(HwTileRam& tileram, int16_t objXa, uint8_t paletteIdx, HwPaletteInfo& paletteInfo, vector<uint32_t>& buffer) -> uint8_t
{
  uint8_t paletteBank = paletteInfo.bank;
  uint8_t paletteNo = paletteInfo.no;
  uint8_t paletteOffset;
  if (paletteInfo.mode == HW_PALETTE_MODE_256) {
    paletteOffset = paletteIdx;
  } else /* if (paletteInfo.mode == HW_PALETTE_MODE_16) */ {
    paletteOffset = paletteIdx + paletteNo * 16;
  }
  HwColor spColor = tileram.palette[paletteBank].color[paletteOffset];  // Memory Access
  HwColor bufColor = {.data = buffer[objXa]};
  color_merge(bufColor, spColor);
  buffer[objXa] = bufColor.data;
  return true;
}

auto colorMergeBG(HwTileRam& tileram, int16_t objXa, uint8_t paletteIdx, HwPaletteInfo& paletteInfo, vector<uint32_t>& buffer) -> uint8_t
{
  uint8_t paletteBank = paletteInfo.bank;
  uint8_t paletteNo = paletteInfo.no;
  uint8_t paletteOffset;
  if (paletteInfo.mode == HW_PALETTE_MODE_256) {
    paletteOffset = paletteIdx;
  } else /* if (paletteInfo.mode == HW_PALETTE_MODE_16) */ {
    paletteOffset = paletteIdx + paletteNo * 16;
  }
  HwColor bgColor = tileram.palette[paletteBank].color[paletteOffset];  // Memory Access
  buffer[objXa] = bgColor.data;
  return true;
}

auto Vpu::drawLineRtl(int y) -> void
{
  HwTileRam& tileram = *(HwTileRam*)memory.section("tile").buffer();
  uint8_t* vram = (uint8_t*)memory.section("vram").buffer();
  auto spWrittenFlag = vector<uint8_t>(HW_SCREEN_W);
  uint32_t fillColor = 0x00000000;  // Memory Access
  for (int x=0; x<HW_SCREEN_W; x++) {
    int layer = 0;
    lineBufferSp[layer][x] = fillColor;
  }
  for (auto& buf: lineBufferBg) {
    std::fill(buf.begin(), buf.end(), 0);
  }
  int lineSPCycles = 0;
  lineSPCycles += 5 + 4;
  if (debug.enableSp.v1) {
    for (int i=0; i<HW_SPRITE_NUM; i++) {
      HwSprite& sp = tileram.sp[0].sprite[i];  // Memory Access * sp_num
      if (!sp.enable) continue;
      if (!isInDrawAreaY(sp.tileSize, sp.x, sp.y)) continue;
      int layer = 0;
      uint32_t x0x1 = getDrawAreaX(sp.tileSize, sp.x, sp.affineEnable);
      int16_t beginX = x0x1 >> 16, endX = x0x1 & 0xFFFF;
      for (int x=beginX; x<endX; x++) {
        // if (!spWrittenFlag[x]) {
        auto xaya = affineTransform(x, y, sp.x, sp.y, sp.affineEnable, sp.affineInv);
        int16_t xa = xaya >> 16, ya = xaya & 0xFFFF;
        auto tileIdx = getTileIdxSP(tileram, sp.tileSize, xa, ya, sp.tileIdx);
        auto paletteIdx = getPaletteIdx(tileram, xa, ya, sp.tileBank, tileIdx);
        spWrittenFlag[x] = colorMergeSP(tileram, sp.x, paletteIdx, sp.paletteInfo, lineBufferSp[layer]);
        lineSPCycles += 1;
        // }
      }
    }
  }
  int idxs[4] = {0, 1, 2, 3};
  int layers[4];
  for (int i=0; i<4; i++) layers[i] = tileram.bg[i].layer;
  sortLayerIndex(idxs, layers);
  int lineBGCycles = 0;
  lineBGCycles += 5 + 5;
  for (int i=0; i<4; i++) {
    uint8_t layer = idxs[i];
    auto& bg = tileram.bg[layer];
    if (debug.enableBg[layer].v1 && bg.enable) {
      for (int x=0; x<HW_SCREEN_W; x++) {
        if (bg.mode == HW_BG_PIXEL_MODE) {
          drawLineBGPixel(vram, tileram.bg[layer], x, y, lineBufferBg[layer]);
        } else {
          auto xaya = affineTransform(x, y, bg.x, bg.y, bg.affineEnable, bg.affineInv);
          int16_t xa = xaya >> 16, ya = xaya & 0xFFFF;
          // if (xa < 0 || xa >= 512) continue;
          // if (ya < 0 || ya >= 512) continue;
          auto tileIdx = getTileIdxBG(tileram, bg.tileSize, xa, ya, bg.tilemapBank);
          auto paletteIdx = getPaletteIdx(tileram, xa, ya, bg.tileBank, tileIdx);
          colorMergeBG(tileram, x, paletteIdx, bg.paletteInfo, lineBufferBg[layer]);
          lineBGCycles += 1;
        }
      }
    }
  }
  fmt::print("y={}, cycles SP={}, BG={} Total={}\n",
    y, lineSPCycles, lineBGCycles, lineSPCycles+lineBGCycles);
  for (int x=0; x<HW_SCREEN_W; x++) {
    HwColor finalColor = {.data = lineBufferSp[0][x]};
    for (int layer=0; layer<4; layer++) {
      HwColor spColor = {.data=lineBufferSp[layer][x]};
      if (color_merge(finalColor, spColor)) /*break*/;
      HwColor bgColor = {.data=lineBufferBg[layer][x]};
      if (color_merge(finalColor, bgColor)) /*break*/;
    }
    finalColor.a = 255;
    screenBuffer[x + y*HW_SCREEN_W] = finalColor.data;
  }
}

auto Vpu::drawAllLine() -> void
{
  for (int y=0; y<HW_SCREEN_H; y++) {
    drawLine(y);
  }
}

auto Vpu::copyScreenBuffer(uint32_t* buffer, bool inv) -> void
{
  if (inv) {
    for (int i=0; i<HW_SCREEN_W * HW_SCREEN_H; i++) {
      uint32_t color = screenBuffer[i];
      color = (
          ((color & 0xFF000000u) >> 24) |
          ((color & 0x00FF0000u) >> 8) |
          ((color & 0x0000FF00u) << 8) |
          ((color & 0x000000FFu) << 24)
      );
      buffer[i] = color;
    }
  } else {
    for (int i=0; i<HW_SCREEN_W * HW_SCREEN_H; i++) {
      uint32_t color = screenBuffer[i];
      buffer[i] = color;
    }
  }
}
