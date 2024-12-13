#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nkx/hw/nkx_hw.h>
#include <nkx/mw/nkx_mw.h>

extern uint8_t _binary_rsc_back_gif_pal_start;
extern uint8_t _binary_rsc_back_gif_pal_size;
extern uint8_t _binary_rsc_back_gif_tile_start;
extern uint8_t _binary_rsc_back_gif_tile_size;

extern uint8_t _binary_rsc_tileset_gif_pal_start;
extern uint8_t _binary_rsc_tileset_gif_pal_size;
extern uint8_t _binary_rsc_tileset_gif_tile_start;
extern uint8_t _binary_rsc_tileset_gif_tile_size;

const uint16_t ground_map[16][2] = {
  {4, 5}, {36, 37}, {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99},
  {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99},
};

int32_t camera_x = 0;
int8_t padIdleCount = 0;
bool isVsyncIntr = false;

extern "C" void int_handler()
{
  // [TODO] Should the status flag be accessed each time it is checked?
  volatile HwIoInterruptBits& intstatus = (*(volatile HwIoInterruptBits*)HWREG_IO_INT_STATUS_ADDR);

  if (intstatus.hblank) {
    intstatus.hblank = 0;
  }
  if (intstatus.vblank) {
    intstatus.vblank = 0;
    isVsyncIntr = true;
  }
}

void waitForVSync()
{
  while (!isVsyncIntr) {
    asm("nop");
    asm("wfi");
    asm("nop");
  }
  isVsyncIntr = false;
}

int main()
{
  HwIoRam& ioram = *(HwIoRam*)HWREG_IORAM_BASEADDR;
  ioram.intr.enable.hblank = true;
  ioram.intr.enable.vblank = true;
  // ioram.intr.enable.dma0 = true;
  // ioram.intr.enable.dma1 = true;
  // ioram.intr.enable.dma2 = true;
  // ioram.intr.enable.dma3 = true;
  // ioram.intr.enable.timer0 = true;
  // ioram.intr.enable.timer1 = true;
  // ioram.intr.enable.timer3 = true;
  // ioram.intr.enable.timer4 = true;

  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  /* palette settings */
  nkx::setPalette(0, 1, 0,
    (uint32_t*)&_binary_rsc_back_gif_pal_start,
    (size_t)   &_binary_rsc_back_gif_pal_size
  );
  nkx::setPalette(0, 2, 0,
    (uint32_t*)&_binary_rsc_tileset_gif_pal_start,
    (size_t)   &_binary_rsc_tileset_gif_pal_size
  );

  /* tile settings */
  nkx::setTile(1, 0,
    (uint8_t*)&_binary_rsc_back_gif_tile_start,
    (size_t)  &_binary_rsc_back_gif_tile_size
  );
  nkx::setTile(3, 0,
    (uint8_t*)&_binary_rsc_tileset_gif_tile_start,
    (size_t)  &_binary_rsc_tileset_gif_tile_size
  );

  /* tilemap settings */
  nkx::setTilemap2DLambda(1,
      HW_TILEMAP_XTILE, HW_TILEMAP_YTILE,
      0, 0,
      [](int32_t x, int32_t y){return (uint16_t)(x + y*48);},
      48, 48,
      0, 0, 64, 30
  );
  nkx::setTilemap2D(2,
      HW_TILEMAP_XTILE, HW_TILEMAP_YTILE,
      0, 24,
      (const uint16_t*)ground_map,
      2, 16,
      0, 0, 64, 16
  );

  /* BG settings */
  tileram.bg[0].enable = true;
  tileram.bg[0].layer = 0;
  tileram.bg[0].paletteInfo.mode = HW_PALETTE_MODE_16;
  tileram.bg[0].paletteInfo.bank = 0;
  tileram.bg[0].paletteInfo.no = 1;
  tileram.bg[0].paletteInfo.transparency = 0;
  tileram.bg[0].tileBank = 1;
  tileram.bg[0].tilemapBank = 1;
  tileram.bg[0].x = 0;
  tileram.bg[0].y = 0;

  tileram.bg[1].enable = true;
  tileram.bg[1].layer = 1;
  tileram.bg[1].paletteInfo.mode = HW_PALETTE_MODE_16;
  tileram.bg[1].paletteInfo.bank = 0;
  tileram.bg[1].paletteInfo.no = 2;
  tileram.bg[1].paletteInfo.transparency = 0;
  tileram.bg[1].tileBank = 3;
  tileram.bg[1].tilemapBank = 2;
  tileram.bg[1].x = 0;
  tileram.bg[1].y = 0;

  /* main loop */
  while (1) {
    waitForVSync();

    volatile uint32_t pad = (*(volatile uint32_t*)HWREG_IO_PAD0_ADDR);
    volatile uint16_t scanline = (*(volatile uint16_t*)HWREG_IO_SCANLINE_ADDR);

    if (pad == 0) {
      if (padIdleCount < 60 * 2) {
        padIdleCount++;
      } else {
        camera_x++;
      }
    } else {
      int speed = 1;
      if (pad & (1 << HW_PAD_B)) {
        speed = 2;
      }
      if (pad & (1 << HW_PAD_LEFT)) {
        camera_x -= speed;
      } else
      if (pad & (1 << HW_PAD_RIGHT)) {
        camera_x += speed;
      }
      padIdleCount = 0;
    }
    int camera_bg3_x = camera_x / 16;
    tileram.bg[1].x = -camera_x / 1 % 512;
    tileram.bg[0].x = -camera_bg3_x % 512;

    for (int y=0; y<30; y++) {
      auto tilemapBank = tileram.bg[0].tilemapBank;
      int tileX = (camera_bg3_x + HW_SCREEN_W) % 512 / 8;
      int texX = (camera_bg3_x + HW_SCREEN_W) % 384 / 8;
      tileram.tilemap[tilemapBank].tileIdx[tileX + y*HW_TILEMAP_XTILE].idx = texX + y*48;
    }
  }

  return 0;
}
