#include <memory>
#include <vector>
#include <fmt/core.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <SDL2/SDL.h>
#include "Varty_a7_35t_simple_ili9341_parallel_8bit.h"


bool handleInput()
{
  // handle events
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type) {
    case SDL_QUIT: /* Window [X] button, or Ctrl-Q */
      return true;
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      // case SDLK_e: board.io.pressPadButton(HW_PAD_UP); break;
      default:
        break;
      }
      break;
    case SDL_KEYUP:
      switch (event.key.keysym.sym) {
      // case SDLK_e: board.io.releasePadButton(HW_PAD_UP); break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  return false;
}


int main(int argc, char **argv) {
  // Verilatorのコマンド解析
  std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
  contextp->commandArgs(argc, argv);

  // DUTモジュールのインスタンス化
  std::unique_ptr<Varty_a7_35t_simple_ili9341_parallel_8bit> dut{
    new Varty_a7_35t_simple_ili9341_parallel_8bit{contextp.get(), "dut"}
  };

  // DUTモジュールのインタフェースの初期化
  contextp->timeunit(-9);  // Set timeunit to 1ns
  contextp->timeprecision(-12);  // Set timeprecision to 1ns
  // contextp->traceEverOn(true);  // Allow traces
  // std::unique_ptr<VerilatedVcdC> tfp{new VerilatedVcdC};

  // dut->trace(tfp.get(), 100);
  // tfp->open("simx.vcd");

  SDL_Init(
    SDL_INIT_VIDEO
    // | SDL_INIT_AUDIO
  );
  int HW_SCREEN_W = 320;
  int HW_SCREEN_H = 240;
  SDL_Window *window;
  window = SDL_CreateWindow( 
    "test verilator",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    HW_SCREEN_W,
    HW_SCREEN_H,
    SDL_WINDOW_SHOWN /*| SDL_WINDOW_OPENGL*/ | SDL_WINDOW_RESIZABLE
  );
  if(window==NULL){
    return 1;
  }
  int screenScale = 3;
  SDL_SetWindowSize(window, HW_SCREEN_W * screenScale, HW_SCREEN_H * screenScale);
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_Texture* texture = SDL_CreateTexture(renderer, 
        SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, HW_SCREEN_W, HW_SCREEN_H);

  std::vector<uint8_t> pixels(HW_SCREEN_W * HW_SCREEN_H * 3);

  uint64_t time_counter = 0;

  // init
  dut->CLK100MHZ = 0;
  dut->btn = 0;
  dut->ck_a5   = 0;
  dut->ck_a4   = 0;  // tft_rst
  dut->ck_a3   = 0;  // tft_cs
  dut->ck_a2   = 0;  // tft_rs  0:data, 1:cmd
  dut->ck_a1   = 0;  // tft_wr
  dut->ck_a0   = 0;  // tft_rd
  dut->ck_io8  = 0;
  dut->ck_io9  = 0;
  dut->ck_io2  = 0;
  dut->ck_io3  = 0;
  dut->ck_io4  = 0;
  dut->ck_io5  = 0;
  dut->ck_io6  = 0;
  dut->ck_io7  = 0;
  dut->led = 0;

  bool draw_start = false;
  int clk10mhz0 = 0, clk10mhz = 0;
  int px_ptr = 0;
  bool is_lo = false;
  uint8_t data_lo, data_hi;
  while (time_counter < 10000000) {
    // update clock
    if ((time_counter % 5) == 0) {
      dut->CLK100MHZ = !dut->CLK100MHZ;
    }
    if ((time_counter % 10) == 0) {
      clk10mhz0 = clk10mhz;
      clk10mhz = !clk10mhz;
    }

    // reset
    if (time_counter ==   40) dut->btn = 1;  // btn[0] = 1;
    if (time_counter ==  160) dut->btn = 0;  // btn[0] = 0;
    // if (time_counter == 1000) dut->btn = 2;  // btn[1] = 1;
    // if (time_counter == 1120) dut->btn = 0;  // btn[1] = 1;

    dut->eval();
    // tfp->dump(contextp->time());
    contextp->timeInc(1);
    time_counter++;

    uint8_t data8 = (dut->ck_io7 << 7)
                  | (dut->ck_io6 << 6)
                  | (dut->ck_io5 << 5)
                  | (dut->ck_io4 << 4)
                  | (dut->ck_io3 << 3)
                  | (dut->ck_io2 << 2)
                  | (dut->ck_io9 << 1)
                  | (dut->ck_io8 << 0);

    if (dut->ck_a2 && data8 == 0x2c) {
      draw_start = true;
    }
    // fmt::print("ck_a2=0x{:x} data8=0x{:02x}\n", dut->ck_a2, data8);
    if (draw_start && (time_counter % 40) == 0) {
      // fmt::print("data8=0x{:02x}\n", data8);
      if (is_lo) {
        data_lo = data8;
      } else {
        data_hi = data8;
        pixels[px_ptr * 3 + 0] = data_lo & 0x1f;
        pixels[px_ptr * 3 + 1] = ((data_hi & 0x7) << 3) | ((data_lo >> 5) & 0x7);
        pixels[px_ptr * 3 + 2] = data_hi >> 3;
        SDL_UpdateTexture(texture, NULL, pixels.data(), HW_SCREEN_W * 3);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL); // ウィンドウいっぱいに引き伸ばして表示
        SDL_RenderPresent(renderer);
      }
    }
    if ((time_counter % 80) == 0) {
      px_ptr += 1;
      if (px_ptr == HW_SCREEN_W * HW_SCREEN_H)
        px_ptr = 0;
      is_lo = !is_lo;
    }
    fmt::print("x,y = {}, {}\n", (px_ptr % 320), (px_ptr / 320));

    if (handleInput()) break;

    // SDL_Delay(16); // 約60fps
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  dut->final();
  // tfp->close();
  return 0;
}
