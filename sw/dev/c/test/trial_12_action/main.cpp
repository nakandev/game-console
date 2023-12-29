#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nkx/hw/nkx_hw.h>
#include <nkx/mw/nkx_mw.h>

extern uint8_t _binary_img_back_gif_pal_start;
extern uint8_t _binary_img_back_gif_pal_size;
extern uint8_t _binary_img_back_gif_tile_start;
extern uint8_t _binary_img_back_gif_tile_size;

extern uint8_t _binary_img_tileset_gif_pal_start;
extern uint8_t _binary_img_tileset_gif_pal_size;
extern uint8_t _binary_img_tileset_gif_tile_start;
extern uint8_t _binary_img_tileset_gif_tile_size;

const uint16_t ground_map[16][2] = {
  {4, 5}, {36, 37}, {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99},
  {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99},
};

int32_t camera_x = 0;
int8_t padIdleCount = 0;

extern "C" void int_handler()
{
  HwTileRam& tileram = (*(HwTileRam*)HWREG_TILERAM_BASEADDR);
  volatile uint32_t intstatus = (*(volatile uint32_t*)HWREG_IO_INT_STATUS_ADDR);
  volatile uint16_t scanline = (*(volatile uint16_t*)HWREG_IO_SCANLINE_ADDR);
  volatile uint32_t pad = (*(volatile uint32_t*)HWREG_IO_PAD0_ADDR);

  if (intstatus & (1<<HW_IO_INT_HBLANK)) {
  }
  if (intstatus & (1<<HW_IO_INT_VBLANK)) {
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
    tileram.bg[2].x = -camera_x / 1 % 512;
    tileram.bg[3].x = -camera_bg3_x % 512;

    for (int y=0; y<30; y++) {
      auto tilemapNo = tileram.bg[3].tilemapNo;
      int tileX = (camera_bg3_x + HW_SCREEN_W) % 512 / 8;
      int texX = (camera_bg3_x + HW_SCREEN_W) % 384 / 8;
      tileram.tilemap[tilemapNo].tileIdx[tileX + y*HW_TILEMAP_XTILE].data = texX + y*48;
    }
  }
}

int main()
{
  HwIoRam& ioram = *(HwIoRam*)HWREG_IORAM_BASEADDR;
  ioram.intr.enable.bits = (1<<HW_IO_INT_HBLANK) | (1<<HW_IO_INT_VBLANK);

  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  /* palette settings */
  nkx::setPalette(0, 1, 0,
    (uint32_t*)&_binary_img_back_gif_pal_start,
    (size_t)   &_binary_img_back_gif_pal_size
  );
  nkx::setPalette(0, 2, 0,
    (uint32_t*)&_binary_img_tileset_gif_pal_start,
    (size_t)   &_binary_img_tileset_gif_pal_size
  );

  /* tile settings */
  nkx::setTile(1, 0,
    (uint8_t*)&_binary_img_back_gif_tile_start,
    (size_t)  &_binary_img_back_gif_tile_size
  );
  nkx::setTile(3, 0,
    (uint8_t*)&_binary_img_tileset_gif_tile_start,
    (size_t)  &_binary_img_tileset_gif_tile_size
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
  tileram.bg[3].flag.enable = true;
  // tileram.bg[2].flag.layer = 2;
  tileram.bg[3].paletteInfo.mode = 1;
  tileram.bg[3].paletteInfo.bank = 0;
  tileram.bg[3].paletteInfo.no = 1;
  tileram.bg[3].tileNo = 1;
  tileram.bg[3].tilemapNo = 1;
  tileram.bg[3].x = 0;
  tileram.bg[3].y = 0;
  tileram.bg[2].flag.enable = true;
  // tileram.bg[2].flag.layer = 3;
  tileram.bg[2].paletteInfo.mode = 1;
  tileram.bg[2].paletteInfo.bank = 0;
  tileram.bg[2].paletteInfo.no = 2;
  tileram.bg[2].tileNo = 3;
  tileram.bg[2].tilemapNo = 2;
  tileram.bg[2].x = 0;
  tileram.bg[2].y = 0;

  /* main loop */
  while (1) {
  }

  return 0;
}
