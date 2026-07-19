package gameconsole_pkg;

parameter SCREEN_W      = 320;
parameter SCREEN_H      = 200;
parameter SCREEN_HBLANK =  80;
parameter SCREEN_VBLANK =  50;
// parameter SCREEN_VBLANK =  23;  // VGA: 525 line => 262.5
parameter HW_TILE_W = 8;
parameter HW_TILE_H = 8;
parameter HW_TILEMAP_W = 512;  // 64[tile-x]
parameter HW_TILEMAP_H = 256;  // 32[tile-y]
parameter PRM_ADDR_W    = 10;
parameter PRM_DATA_W    =  32;
// parameter MAP_BANK_W    =   3;  // 8[bank] = 2^3
parameter MAP_BANK_W    =   1;  // 2[bank] = 2^1  [DEBUG]
parameter MAP_INDX_W    =  11;  // 64[tile-x] * 32[tile-y] = 2048[tile] = 2^11
parameter MAP_ADDR_W    = MAP_BANK_W + MAP_INDX_W;
parameter MAP_DATA_W    =  16;
// parameter TILE_BANK_W   =   3;  // 8[bank] = 2^3
parameter TILE_BANK_W   =   2;  // 4[bank] = 2^2  [DEBUG]
parameter TILE_INDX_W   =   8 + 6;  // 256[tile] * 64[pix] = 2^(8 + 6)
parameter TILE_ADDR_W   = TILE_BANK_W + TILE_INDX_W;
parameter TILE_DATA_W   =   8;
parameter PAL_BANK_W    =   1;  // 2[bank] = 2^1
parameter PAL_INDX_W    =   8;
parameter PAL_ADDR_W    = PAL_BANK_W + PAL_INDX_W;
parameter PAL_DATA_W    =  32;
parameter LINEBUFF_BANK_W = 1;  // 0..1 = 2^1
parameter LINEBUFF_ADDR_W = 9;  // 0..(320)..511 = 2^9
parameter LINEBUFF_DATA_W = 32;

endpackage
