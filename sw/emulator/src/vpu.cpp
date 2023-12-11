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

void Vpu::init()
{
}

int Vpu::currentLineNo()
{
  return _currentLineNo;
}

static const int widthTable[] = {8, 16, 32, 64};
static const int heightTable[] = {8, 16, 32, 64};
static int prepareVpuSprite(HwTileRam& tileram, vector<VpuSprite>& vpusp, int y)
{
  int spriteNum = 0;
  for (int i=0; i<HW_SPRITE_NUM; i++) {
    HwSprite& sp = tileram.sp[0].sprite[i];
    if (!sp.flag.enable) continue;
    int16_t w = widthTable[sp.tileSize];
    int16_t h = heightTable[sp.tileSize];
    int16_t beginX = sp.x;
    int16_t beginY = sp.y;
    int16_t endX = sp.x + w - 1;
    int16_t endY = sp.y + h - 1;
    if (sp.flag.affineEnable) {
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

static bool color_merge(HwColor& src, const HwColor& dst){
  uint8_t a = dst.a;
  if (a == 0) return false;
  if (a == 255) {
    src.data = dst.data;
    return true;
  } else {
    src.r = (uint8_t)(((uint16_t)src.r * (255 - a) + (uint16_t)dst.r * a) / 256);
    src.g = (uint8_t)(((uint16_t)src.g * (255 - a) + (uint16_t)dst.g * a) / 256);
    src.b = (uint8_t)(((uint16_t)src.b * (255 - a) + (uint16_t)dst.b * a) / 256);
    src.a = (src.a + dst.a) / 2;
    return false;
  }
}

static void drawLineBGPixel(uint8_t* vram, HwBG& bg, int x, int y, vector<uint32_t>& buffer)
{
  HwColor* colors = (HwColor*) vram;
  buffer[x] = colors[x + y * HW_SCREEN_W].data;
}

static void drawLineBGTile(HwTileRam& tileram, HwBG& bg, int x, int y, vector<uint32_t>& buffer)
{
  uint8_t tilemapNo = bg.tilemapNo;
  uint8_t tileNo = bg.tileNo;
  uint8_t paletteNo = bg.paletteNo;
  int32_t x0 = x - bg.x;
  int32_t y0 = y - bg.y;
  if (bg.flag.affineEnable) {
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
  uint16_t offset = (tx0 / HWTILE_W) + (ty0 / HWTILE_H) * HW_TILEMAP_XTILE;
  uint16_t tileIdx = tileram.tilemap[tilemapNo].tileIdx[offset];
  uint8_t paletteIdx = tileram.tile[tileNo][tileIdx].data[ty0 % HWTILE_H][tx0 % HWTILE_W];
  buffer[x] = tileram.palette[paletteNo].color[paletteIdx].data;
}

static void drawLineSprite(HwTileRam& tileram, VpuSprite& vpusp, int x, int y, vector<uint32_t>& buffer)
{
  HwSprite& sp = vpusp.hwsp;
  uint8_t tileNo = sp.tileNo;
  int32_t x0 = x - sp.x;
  int32_t y0 = y - sp.y;
  if (sp.flag.affineEnable) {
    HwMatrix2d B = sp.affineInv;  // B = inv(A)
    int32_t x00 = x0 - B.x;
    int32_t y00 = y0 - B.y;
    // float div = B.a/256. * B.d/256. - B.b/256. * B.c/256.;
    // x0 = (x00 * +B.d/256. + y00 * -B.b/256.) / div + B.x;
    // y0 = (x00 * -B.c/256. + y00 * +B.a/256.) / div + B.y;
    x0 = (x00 * B.a/256. + y00 * B.b/256.) + B.x;
    y0 = (x00 * B.c/256. + y00 * B.d/256.) + B.y;
    if (x0 < 0 || x0 >= widthTable[sp.tileSize]) return;
    if (y0 < 0 || y0 >= heightTable[sp.tileSize]) return;
  }
  uint8_t offset = (x0 / HWTILE_W) + (y0 / HWTILE_H) * (widthTable[sp.tileSize] / HWTILE_W);
  uint8_t tileIdx = sp.tileIdx + offset;
  uint8_t paletteNo = sp.paletteNo;
  uint32_t tilex = ((uint32_t)x0) % HW_TILEMAP_W % HWTILE_W;
  uint32_t tiley = ((uint32_t)y0) % HW_TILEMAP_H % HWTILE_H;
  uint8_t paletteIdx = tileram.tile[tileNo][tileIdx].data[tiley][tilex];
  HwColor spColor = tileram.palette[paletteNo].color[paletteIdx];
  HwColor bufColor = {.data = buffer[x]};
  color_merge(bufColor, spColor);
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

void Vpu::drawLine(int y)
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
  for (int i=0; i<4; i++) layers[i] = tileram.bg[i].flag.layer;
  sortLayerIndex(idxs, layers);
  for (int x=0; x<HW_SCREEN_W; x++) {
    for (int i=0; i<4; i++) {
      uint8_t layer = idxs[i];
      auto& bg = tileram.bg[layer];
      if (debug.enableBg[layer].v1 && bg.flag.enable) {
        if (bg.flag.mode == HWBG_PIXEL_MODE) {
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
        uint8_t layer = vpusp.hwsp.flag.layer;
        if (x < vpusp.beginX || vpusp.endX < x) continue;
        drawLineSprite(tileram, vpusp, x, y, lineBufferSp[layer]);
      }
    }
  }
  for (int x=0; x<HW_SCREEN_W; x++) {
    HwColor finalColor = {.data = 0};
    for (int i=0; i<4; i++) {
      HwColor spColor = {.data=lineBufferSp[i][x]};
      if (color_merge(finalColor, spColor)) break;
      HwColor bgColor = {.data=lineBufferBg[i][x]};
      if (color_merge(finalColor, bgColor)) break;
    }
    finalColor.a = 255;

    screenBuffer[x + y*HW_SCREEN_W] = finalColor.data;
  }
}

void Vpu::drawAllLine()
{
  for (int y=0; y<HW_SCREEN_H; y++) {
    drawLine(y);
  }
}

void Vpu::copyScreenBuffer(uint32_t* buffer, bool inv)
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
