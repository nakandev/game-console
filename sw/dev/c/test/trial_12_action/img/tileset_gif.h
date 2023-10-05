#pragma once
#include <cstdint>
#ifndef arrayof
#define arrayof(x) (sizeof(x)/sizeof(x[0]))
#endif
extern "C" {
extern uint32_t tileset_gif_pal[17];
extern uint8_t tileset_gif_data[36864];
};
