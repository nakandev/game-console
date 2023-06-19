#include <SDL2/SDL.h>
#include <emulator.h>
#include <fmt/core.h>

bool handleInput(Board& board)
{
  // handle events
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_QUIT: /** ウィンドウのxボタンやctrl-Cを押した場合 */
    return true;
  case SDL_KEYDOWN:
    switch (event.key.keysym.sym) {
    case SDLK_e: board.io.pressPadButton(PAD_UP); break;
    case SDLK_d: board.io.pressPadButton(PAD_DOWN); break;
    case SDLK_f: board.io.pressPadButton(PAD_LEFT); break;
    case SDLK_s: board.io.pressPadButton(PAD_RIGHT); break;
    case SDLK_l: board.io.pressPadButton(PAD_A); break;
    case SDLK_k: board.io.pressPadButton(PAD_B); break;
    case SDLK_i: board.io.pressPadButton(PAD_C); break;
    case SDLK_j: board.io.pressPadButton(PAD_D); break;
    case SDLK_h: board.io.pressPadButton(PAD_S); break;
    case SDLK_g: board.io.pressPadButton(PAD_T); break;
    default:
      break;
    }
    break;
  case SDL_KEYUP:
    switch (event.key.keysym.sym) {
    case SDLK_e: board.io.releasePadButton(PAD_UP); break;
    case SDLK_d: board.io.releasePadButton(PAD_DOWN); break;
    case SDLK_f: board.io.releasePadButton(PAD_LEFT); break;
    case SDLK_s: board.io.releasePadButton(PAD_RIGHT); break;
    case SDLK_l: board.io.releasePadButton(PAD_A); break;
    case SDLK_k: board.io.releasePadButton(PAD_B); break;
    case SDLK_i: board.io.releasePadButton(PAD_C); break;
    case SDLK_j: board.io.releasePadButton(PAD_D); break;
    case SDLK_h: board.io.releasePadButton(PAD_S); break;
    case SDLK_g: board.io.releasePadButton(PAD_T); break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  return false;
}

int main()
{
  string path = "/home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/trial_02_tileram/trial_02_tileram";
  auto board = Board();

  board.cpu.setMaxCycles(100);
  board.cpu.loadElf(path);
  board.cpu.initRegs();
  board.cpu.setEntrypoint(0x0800'0000);

  SDL_Init(SDL_INIT_VIDEO);   // Initialize SDL2

  SDL_Window *window;        // Declare a pointer to an SDL_Window
  window = SDL_CreateWindow( 
    "An SDL2 window",                  //    window title
    SDL_WINDOWPOS_UNDEFINED,           //    initial x position
    SDL_WINDOWPOS_UNDEFINED,           //    initial y position
    320,                               //    width, in pixels
    240,                               //    height, in pixels
    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
  );
  if(window==NULL){
    // std::cout << "Could not create window: " << SDL_GetError() << '\n';
    return 1;
  }
  SDL_Renderer* renderer;
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_Texture* texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGBA8888,
    // SDL_PIXELFORMAT_ABGR8888,
    SDL_TEXTUREACCESS_STREAMING,
    320, 240
  );
  SDL_RenderSetLogicalSize(renderer, 320, 240);
  SDL_RenderSetScale(renderer, 2.0, 2.0);

  board.updateFrameUntilVblank();
  board.updateFrameSinceVblank();
  int pitch = 320 * 4;
  float fps = 60.0;
  int sec = 30;
  float msecPerFrame = 1000.0/fps;
  uint32_t startTime, endTime;
  for (int i=0; i<sec*fps; i++) {
    startTime = SDL_GetTicks();
    if (handleInput(board)) {
      break;
    }

    // update cpu & screen
    uint32_t* screenBuffer = nullptr;
    SDL_LockTexture(texture, NULL, (void**)&screenBuffer, &pitch);

    board.updateFrameUntilVblank();
    board.ppu.copyScreenBuffer(screenBuffer);
    board.updateFrameSinceVblank();

    SDL_UnlockTexture(texture);
    SDL_UpdateTexture(texture, NULL, screenBuffer, pitch);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    endTime = SDL_GetTicks();
    // fmt::print("ticks:{}/{:.2f}\n", endTime - startTime, msecPerFrame);
    if (startTime + msecPerFrame > endTime) {
      SDL_Delay((uint32_t)(startTime + msecPerFrame - endTime));
    }
  }

  // The window is open: enter program loop (see SDL_PollEvent)
  if (texture) SDL_DestroyTexture(texture);
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();


  return 0;
}
