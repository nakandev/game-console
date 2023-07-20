#include <SDL2/SDL.h>
#include <emulator.h>
#include <fmt/core.h>
#include <cmath>

static uint nearPow2(int n)
{
  if (n <= 0) return 0;
  if ((n & (n - 1)) == 0) return (uint)n;
  uint ret = 1;
  while (n > 0) { ret <<= 1; n >>= 1; }
  return ret;
}

bool handleInput(Board& board)
{
  // handle events
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: /** ウィンドウのxボタンやctrl-Cを押した場合 */
      return true;
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      case SDLK_e: board.io.pressPadButton(HW_PAD_UP); break;
      case SDLK_d: board.io.pressPadButton(HW_PAD_DOWN); break;
      case SDLK_s: board.io.pressPadButton(HW_PAD_LEFT); break;
      case SDLK_f: board.io.pressPadButton(HW_PAD_RIGHT); break;
      case SDLK_l: board.io.pressPadButton(HW_PAD_A); break;
      case SDLK_k: board.io.pressPadButton(HW_PAD_B); break;
      case SDLK_i: board.io.pressPadButton(HW_PAD_C); break;
      case SDLK_j: board.io.pressPadButton(HW_PAD_D); break;
      case SDLK_h: board.io.pressPadButton(HW_PAD_S); break;
      case SDLK_g: board.io.pressPadButton(HW_PAD_T); break;
      case SDLK_o: board.io.pressPadButton(HW_PAD_L); break;
      case SDLK_w: board.io.pressPadButton(HW_PAD_R); break;
      default:
        break;
      }
      break;
    case SDL_KEYUP:
      switch (event.key.keysym.sym) {
      case SDLK_e: board.io.releasePadButton(HW_PAD_UP); break;
      case SDLK_d: board.io.releasePadButton(HW_PAD_DOWN); break;
      case SDLK_s: board.io.releasePadButton(HW_PAD_LEFT); break;
      case SDLK_f: board.io.releasePadButton(HW_PAD_RIGHT); break;
      case SDLK_l: board.io.releasePadButton(HW_PAD_A); break;
      case SDLK_k: board.io.releasePadButton(HW_PAD_B); break;
      case SDLK_i: board.io.releasePadButton(HW_PAD_C); break;
      case SDLK_j: board.io.releasePadButton(HW_PAD_D); break;
      case SDLK_h: board.io.releasePadButton(HW_PAD_S); break;
      case SDLK_g: board.io.releasePadButton(HW_PAD_T); break;
      case SDLK_o: board.io.releasePadButton(HW_PAD_L); break;
      case SDLK_w: board.io.releasePadButton(HW_PAD_R); break;
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

void audio_callback(void *userdata, Uint8 *stream, int len)
{
  ApuMusicData& musicData = *(ApuMusicData*)userdata;
  memset(stream, 0, len);
  memcpy(stream, musicData.buffer, HW_MUSIC_FREQ_PER_FRAME*2*2);
  // for (int i=0; i<len/2; i++) {
  //   if (i%2==0)
  //   fmt::print("{}\n", musicData.buffer[i]);
  // }
  // fmt::print("time={} note={} len={}\n", musicData.freqTime, musicData.noteTime, len);
}

int main(int argc, char* argv[])
{
  string path = "/home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/trial_02_tileram/trial_02_tileram";
  if (argc == 2) {
    path = string(argv[1]);
  }
  auto board = Board();

  board.cpu.setMaxCycles(100);
  board.cpu.loadElf(path);
  board.cpu.initRegs();
  board.cpu.setEntrypoint(0x0800'0000);

  SDL_Init(
    SDL_INIT_VIDEO |
    SDL_INIT_AUDIO
  );   // Initialize SDL2

  SDL_Window *window;        // Declare a pointer to an SDL_Window
  window = SDL_CreateWindow( 
    "niRVana : RISC-V Game Console",                  //    window title
    SDL_WINDOWPOS_UNDEFINED,           //    initial x position
    SDL_WINDOWPOS_UNDEFINED,           //    initial y position
    HW_SCREEN_W,                               //    width, in pixels
    HW_SCREEN_H,                               //    height, in pixels
    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL// | SDL_WINDOW_RESIZABLE
  );
  if(window==NULL){
    return 1;
  }
  SDL_Renderer* renderer;
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_Texture* texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGBA8888,
    // SDL_PIXELFORMAT_ABGR8888,
    SDL_TEXTUREACCESS_STREAMING,
    HW_SCREEN_W, HW_SCREEN_H
  );
  SDL_RenderSetLogicalSize(renderer, HW_SCREEN_W, HW_SCREEN_H);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
  SDL_SetWindowSize(window, HW_SCREEN_W * 2, HW_SCREEN_H * 2);
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  int pitch = HW_SCREEN_W * 4;
  float fps = 60.0;
  int sec = 20;
  float msecPerFrame = 1000.0/fps;

  SDL_AudioSpec desired, obtained;
  desired.freq = HW_MUSIC_FREQUENCY;
  desired.format = AUDIO_S16LSB;
  desired.channels = 2;
  desired.samples = nearPow2(HW_MUSIC_FREQ_PER_FRAME * 2);
  // desired.callback = audio_callback;
  desired.callback = nullptr;
  desired.userdata = &board.apu.apuMusicData;

  auto audioDev = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
  if (audioDev == 0) {
    fmt::print("Cannot open audio device.\n");
    return 1;
  }
  SDL_PauseAudioDevice(audioDev, false);

  board.updateFrameUntilVblank();
  board.updateFrameSinceVblank();
  uint32_t startTime, endTime;
  for (int i=0; i<sec*fps; i++) {
    startTime = SDL_GetTicks();
    if (handleInput(board)) {
      break;
    }

    // update cpu, screen, audio
    uint32_t* screenBuffer = nullptr;
    SDL_LockTexture(texture, NULL, (void**)&screenBuffer, &pitch);

    board.updateFrameUntilVblank();
    board.ppu.copyScreenBuffer(screenBuffer);
    SDL_QueueAudio(audioDev, (void*)board.apu.apuMusicData.buffer, HW_MUSIC_FREQ_PER_FRAME*2*2);
    board.updateFrameSinceVblank();

    SDL_RenderClear(renderer);
    SDL_UnlockTexture(texture);
    SDL_UpdateTexture(texture, NULL, screenBuffer, pitch);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    endTime = SDL_GetTicks();
    if (startTime + msecPerFrame > endTime) {
      SDL_Delay((uint32_t)(startTime + msecPerFrame - endTime));
    }
  }

  SDL_PauseAudioDevice(audioDev, true);
  SDL_CloseAudioDevice(audioDev);
  if (texture) SDL_DestroyTexture(texture);
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
