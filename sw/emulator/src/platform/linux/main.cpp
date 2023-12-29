#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#define okCancelButtonAlignement 1.0f
#include "ImGuiFileDialog.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SDL2/SDL_opengl.h>
#include <emulator.h>
#include <gui.h>
#include <fmt/core.h>
#include <filesystem>

GLuint framebuffer;
GLuint texture;
GLuint renderbuffer;

extern bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);
extern void DrawSplitter(int split_vertically, float thickness, float* size0, float* size1, float min_size0, float min_size1);

static uint nearPow2(int n)
{
  if (n <= 0) return 0;
  if ((n & (n - 1)) == 0) return (uint)n;
  uint ret = 1;
  while (n > 0) { ret <<= 1; n >>= 1; }
  return ret;
}

bool handleInput(MainComponent& mainComponent)
{
  Board& board = mainComponent.board;
  // handle events
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type) {
    case SDL_QUIT: /* Window [X] button, or Ctrl-Q */
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
      case SDLK_g: board.io.pressPadButton(HW_PAD_S); break;
      case SDLK_h: board.io.pressPadButton(HW_PAD_T); break;
      case SDLK_w: board.io.pressPadButton(HW_PAD_L); break;
      case SDLK_o: board.io.pressPadButton(HW_PAD_R); break;
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
      case SDLK_g: board.io.releasePadButton(HW_PAD_S); break;
      case SDLK_h: board.io.releasePadButton(HW_PAD_T); break;
      case SDLK_w: board.io.releasePadButton(HW_PAD_L); break;
      case SDLK_o: board.io.releasePadButton(HW_PAD_R); break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
  for (int i=0; i<SDL_NumJoysticks(); i++) {
    auto &padInfo = mainComponent.inputConfigDialog.padInfo;
    if (!SDL_IsGameController(i)) continue;
    if (padInfo.deviceId != i) continue;
    if (!padInfo.device) continue;
    // SDL_GameController* controller = SDL_GameControllerOpen(i);
    // if (!controller) continue;
    auto gameControllerOnOff = [&](int x) {
      if (SDL_GameControllerGetButton(padInfo.device, static_cast<SDL_GameControllerButton>(padInfo.keyAsigns[x])))
        board.io.pressPadButton(x);
      else
        board.io.releasePadButton(x);
    };
    gameControllerOnOff(HW_PAD_A);
    gameControllerOnOff(HW_PAD_B);
    gameControllerOnOff(HW_PAD_C);
    gameControllerOnOff(HW_PAD_D);
    gameControllerOnOff(HW_PAD_L);
    gameControllerOnOff(HW_PAD_R);
    gameControllerOnOff(HW_PAD_S);
    gameControllerOnOff(HW_PAD_T);
    gameControllerOnOff(HW_PAD_UP);
    gameControllerOnOff(HW_PAD_DOWN);
    gameControllerOnOff(HW_PAD_LEFT);
    gameControllerOnOff(HW_PAD_RIGHT);
    // SDL_GameControllerClose(controller);
  }
  return false;
}

void audio_callback(void *userdata, Uint8 *stream, int len)
{
  ApuMusicData& musicData = *(ApuMusicData*)userdata;
  memset(stream, 0, len);
  memcpy(stream, musicData.buffer, HW_MUSIC_FREQ_PER_FRAME*2*2);
}

int main(int argc, char* argv[])
{
  // debugLevel = 1;
  filesystem::path elfpath = "/home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/tutorial/mw/00_template/tutorial_mw_00_template.exe";
  if (argc == 2) {
    elfpath = string(argv[1]);
  }
  filesystem::path elfdir = elfpath.parent_path();

  auto board = Board();

  SDL_Init(
    SDL_INIT_VIDEO |
    SDL_INIT_AUDIO |
    SDL_INIT_JOYSTICK
  );

  const char* glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_Window *window;
  // string title = "niRVana : RISC-V Game Console";
  string title = "RISC-V Retro Game Console";
  window = SDL_CreateWindow( 
    title.c_str(),
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    HW_SCREEN_W,
    HW_SCREEN_H,
    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
  );
  if(window==NULL){
    return 1;
  }
  SDL_SetWindowSize(window, 1024, 768);
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync
  glewInit();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();
  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  auto mainComponent = MainComponent(board);
  mainComponent.setElfPath(elfpath);

  int pitch = HW_SCREEN_W * 4;
  float fps = 60.0;
  int msecPerFrame[] = {17, 16, 17};  // fps 60
  if (fps == 30.0) {
    msecPerFrame[0] = 33;
    msecPerFrame[1] = 34;
    msecPerFrame[2] = 33;
  }

  SDL_AudioSpec desired, obtained;
  desired.freq = HW_MUSIC_FREQUENCY;
  desired.format = AUDIO_S16LSB;
  desired.channels = 2;
  desired.samples = nearPow2(HW_MUSIC_FREQ_PER_FRAME);
  desired.callback = nullptr;
  desired.userdata = &board.apu.apuMusicData;

  auto audioDev = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
  if (audioDev == 0) {
    fmt::print("Cannot open audio device.\n");
    return 1;
  }
  SDL_PauseAudioDevice(audioDev, false);

  /* create_framebuffer */
  mainComponent.createFramebuffer();

  board.updateFrame();
  uint32_t startTime, endTime;
  int loopCount = 0;
  while (true) {
    startTime = SDL_GetTicks();
    if (handleInput(mainComponent)) {
      break;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    // update cpu, screen, audio
    // board.updateFrameUntilVblank();
    // board.updateFrameSinceVblank();
    board.updateFrame();
    board.vpu.copyScreenBuffer(mainComponent.screenBuffer.data(), true);
    if (!board.pause)
      SDL_QueueAudio(audioDev, (void*)board.apu.apuMusicData.buffer, HW_MUSIC_FREQ_PER_FRAME*2*2);

    auto imguiWindowFlag = 
      ImGuiWindowFlags_MenuBar |
      ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoNav;
    auto pOpen = true;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::Begin("Another Window", &pOpen, imguiWindowFlag);
    mainComponent.renderMenu();
    mainComponent.renderMainPanel();
    ImGui::End();

    // Rendering
    ImGui::Render();
    mainComponent.renderFramebuffer();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);

    endTime = SDL_GetTicks();
    if (startTime + msecPerFrame[loopCount%3] > endTime) {
      SDL_Delay((uint32_t)(startTime + msecPerFrame[loopCount%3] - endTime));
    }
    loopCount++;
    if (loopCount == 3*10000) {
      loopCount = 0;
    }
  }

  // Cleanup
  mainComponent.cleanup();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DeleteContext(gl_context);
  if (audioDev) {
    SDL_PauseAudioDevice(audioDev, true);
    SDL_CloseAudioDevice(audioDev);
  }
  if (window) {
    SDL_DestroyWindow(window);
  }
  // SDL_Quit();

  return 0;
}
