#pragma once
#include <cstdint>
#ifndef arrayof
#define arrayof(x) (sizeof(x)/sizeof(x[0]))
#endif
extern "C" {
extern uint32_t riscv_logo_gif_pal[3];
extern uint8_t riscv_logo_gif_data[51200];
};
