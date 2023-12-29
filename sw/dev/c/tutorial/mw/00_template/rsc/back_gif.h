#pragma once
#include <cstdint>
#ifndef arrayof
#define arrayof(x) (sizeof(x)/sizeof(x[0]))
#endif
extern "C" {
extern uint32_t back_gif_pal[9];
extern uint8_t back_gif_data[92160];
};
