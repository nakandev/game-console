package gameconsole_pkg;

parameter SCREEN_W      = 320;
parameter SCREEN_H      = 240;
parameter SCREEN_HBLANK =  80;
parameter SCREEN_VBLANK =  80;
parameter BG_ADDR_W     =  10;
parameter BG_DATA_W     =  32;
parameter SP_ADDR_W     =  10;
parameter SP_DATA_W     =  32;
parameter MAP_BANK_W    =   4;
parameter MAP_INDX_W    =  12;
parameter MAP_ADDR_W    = MAP_BANK_W + MAP_INDX_W;
parameter MAP_DATA_W    =  16;
parameter TILE_BANK_W   =   4;
parameter TILE_INDX_W   =  16;
parameter TILE_ADDR_W   = TILE_BANK_W + TILE_INDX_W;
parameter TILE_DATA_W   =   8;
parameter PAL_BANK_W    =   2;
parameter PAL_INDX_W    =   8;
parameter PAL_ADDR_W    = PAL_BANK_W + PAL_INDX_W;
parameter PAL_DATA_W    =  32;
parameter LINEBUFF_BANK_W = 1;  // 0..1 = 2^1
parameter LINEBUFF_ADDR_W = 9;  // 0..(320)..511 = 2^9
parameter LINEBUFF_DATA_W = 32;

endpackage
