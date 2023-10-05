#pragma once
#include <cstdint>
#ifndef arrayof
#define arrayof(x) (sizeof(x)/sizeof(x[0]))
#endif
extern "C" {
extern uint32_t img_sysfont8x8_gif_pal[2];
extern uint8_t img_sysfont8x8_gif_data[16384];
};
