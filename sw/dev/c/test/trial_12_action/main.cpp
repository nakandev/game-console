#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nkx/hw/nkx_hw.h>
#include <nkx/sw/nkx_sw.h>

extern uint8_t _binary_img_back_gif_pal_start;
extern uint8_t _binary_img_back_gif_pal_size;
extern uint8_t _binary_img_back_gif_tile_start;
extern uint8_t _binary_img_back_gif_tile_size;

extern uint8_t _binary_img_tileset_gif_pal_start;
extern uint8_t _binary_img_tileset_gif_pal_size;
extern uint8_t _binary_img_tileset_gif_tile_start;
extern uint8_t _binary_img_tileset_gif_tile_size;

int32_t camera_x = 0;

extern "C" void int_handler()
{
  HwTileRam& tileram = (*(HwTileRam*)HWREG_TILERAM_BASEADDR);
  volatile uint32_t intstatus = (*(volatile uint32_t*)HW_IO_INT_STATUS_ADDR);
  volatile uint16_t scanline = (*(volatile uint16_t*)HW_IO_SCANLINE_ADDR);
  volatile uint32_t pad = (*(volatile uint32_t*)HW_IO_PAD0_ADDR);
  // if (pad & (1 << HW_PAD_A)) {
  //   mode = 0;
  // } else
  // if (pad & (1 << HW_PAD_B)) {
  //   mode = 1;
  // }
  // 
  // if (mode == 0) {
  //   tileram.bg[0].x = nkx::sin((scanline + radV) & 0xffu) / 8;
  //   tileram.bg[0].y = 0;
  // } else {
  //   tileram.bg[0].x = 0;
  //   tileram.bg[0].y = nkx::sin((scanline + radV) & 0xffu) / 8;
  // }

  if (intstatus & (1<<HW_IO_INT_HBLANK)) {
  }
  if (intstatus & (1<<HW_IO_INT_VBLANK)) {
    camera_x++;
    int camera_bg3_x = camera_x / 8;
    tileram.bg[2].x = -camera_x / 1 % 512;
    tileram.bg[3].x = -camera_bg3_x % 512;

    for (int y=0; y<30; y++) {
      auto tilemapNo = tileram.bg[3].tilemapNo;
      int tileX = (camera_bg3_x + HW_SCREEN_W) % 512 / 8;
      int texX = (camera_bg3_x + HW_SCREEN_W) % 384 / 8;
      tileram.tilemap[tilemapNo].tileIdx[tileX + y*HW_TILEMAP_XTILE] = texX + y*48;
    }
  }
}

int main()
{
  HwIoRam& ioram = *(HwIoRam*)HWREG_IORAM_BASEADDR;
  ioram.intr.enable = (1<<HW_IO_INT_HBLANK) | (1<<HW_IO_INT_VBLANK);

  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;

  /* palette settings */
  nkx::setPalette(1, 0,
    (uint32_t*)&_binary_img_back_gif_pal_start,
    (size_t)   &_binary_img_back_gif_pal_size
  );
  nkx::setPalette(2, 0,
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
  for (int y=0; y<30; y++) {
    for (int x=0; x<64; x++) {
      tileram.tilemap[1].tileIdx[x + y*HW_TILEMAP_XTILE] = x%48 + y*48;
    }
  }
  const uint16_t ground_map[16][2] = {
    {4, 5}, {36, 37}, {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99},
    {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99}, {66, 67}, {98, 99},
  };
  for (int y=0; y<16; y++) {
    for (int x=0; x<64; x++) {
      tileram.tilemap[2].tileIdx[x + (y+24)*HW_TILEMAP_XTILE] = ground_map[y][x%2];
    }
  }

  /* BG settings */
  tileram.bg[3].flag.enable = true;
  // tileram.bg[2].flag.layer = 2;
  tileram.bg[3].paletteNo = 1;
  tileram.bg[3].tileNo = 1;
  tileram.bg[3].tilemapNo = 1;
  tileram.bg[3].x = 0;
  tileram.bg[3].y = 0;
  tileram.bg[2].flag.enable = true;
  // tileram.bg[2].flag.layer = 3;
  tileram.bg[2].paletteNo = 2;
  tileram.bg[2].tileNo = 3;
  tileram.bg[2].tilemapNo = 2;
  tileram.bg[2].x = 0;
  tileram.bg[2].y = 0;

  /* main loop */
  while (1) {
  }

  return 0;
}
