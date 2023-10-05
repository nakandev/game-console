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

bool handleInput(Board& board)
{
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
  filesystem::path elfpath = "/home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/trial_08_tilefile/trial_08_tilefile";
  if (argc == 2) {
    elfpath = string(argv[1]);
  }
  filesystem::path elfdir = elfpath.parent_path();

  auto board = Board();
  auto disasms = vector<string>();

  board.cpu.setMaxCycles(10'000'000);
  if (!board.cpu.loadElf(elfpath)) {
    disasms= board.cpu.disassembleAll();
  }

  bool guiDebug = false;

  SDL_Init(
    SDL_INIT_VIDEO |
    SDL_INIT_AUDIO
  );

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
  int screenScale = 3;
  SDL_SetWindowSize(window, HW_SCREEN_W * screenScale, HW_SCREEN_H * screenScale);
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

  SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync
  glewInit();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  ImGui::StyleColorsLight();
  // Setup Platform/Renderer backends
  const char* glsl_version = "#version 130";
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  int pitch = HW_SCREEN_W * 4;
  float fps = 60.0;
  int sec = 120;
  // int msecPerFrame[] = {33, 34, 33};  // fps 30
  int msecPerFrame[] = {17, 16, 17};  // fps 60

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
  if (true) {
    int width = 320, height = 240;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      fmt::print("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

  }

  vector<uint32_t> screenBuffer;
  screenBuffer.resize(320 * 240);

  // 800 x 600
  float hMenu = 30;
  float margin = 10;
  float thick = 10;
  // float wSCR = io.DisplaySize.x, hSCR = io.DisplaySize.y;
  float wSCR = HW_SCREEN_W * screenScale - margin, hSCR = HW_SCREEN_H * screenScale - margin - hMenu;
  float wLC = (wSCR*0.8)-thick, hLC = (hSCR*0.8)-thick;
  float wL = (wSCR*0.2)-thick, hL = hLC;
  float wC = (wSCR*0.6)-thick, hC = hLC;
  float wR = (wSCR*0.2), hR = (hSCR*1.0);
  float wB = (wSCR*0.8)-thick, hB = (hSCR*0.2);

  board.updateFrameUntilVblank();
  board.updateFrameSinceVblank();
  uint32_t startTime, endTime;
  // for (int i=0; i<sec*fps; i++) {
  int loopCount = 0;
  while (true) {
    startTime = SDL_GetTicks();
    if (handleInput(board)) {
      break;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // update cpu, screen, audio
    board.updateFrameUntilVblank();
    board.ppu.copyScreenBuffer(screenBuffer.data(), true);
    if (!board.pause)
      SDL_QueueAudio(audioDev, (void*)board.apu.apuMusicData.buffer, HW_MUSIC_FREQ_PER_FRAME*2*2);
    board.updateFrameSinceVblank();

    auto imguiWindowFlag = 
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    auto pOpen = true;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::Begin("Another Window", &pOpen, imguiWindowFlag);
    { /* Menu */
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("System")) {
          if(ImGui::MenuItem("Open", "CTRL+O")) {
            string dir = elfpath;
            ImGuiFileDialog::Instance()->OpenDialog(
              "ChooseFileDlgKey", "Choose File", ".exe,.elf,.out", dir,
              1, nullptr, ImGuiFileDialogFlags_Modal
            );
          }

          ImGui::Separator();
          ImGui::MenuItem("Quit", "CTRL+Q");
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Config")) {
          ImGui::MenuItem("Input", "");
          ImGui::MenuItem("Video", "");
          ImGui::MenuItem("Audio", "");
          ImGui::MenuItem("Network", "");
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Emulator")) {
          if (ImGui::MenuItem("Launch", "")) {
            board.pause = false;
          }
          if (ImGui::MenuItem("Pause", "")) {
            board.pause = true;
          }
          if (ImGui::MenuItem("Reset", "")) {
            board.reset();
            board.cpu.loadElf(elfpath);
          }
          ImGui::Separator();
          ImGui::MenuItem("Debug Mode", "", &guiDebug);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      /* File Dialog */
      if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) 
      {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
          string selectedFile = ImGuiFileDialog::Instance()->GetFilePathName();
          string selectedDir = ImGuiFileDialog::Instance()->GetCurrentPath();
          elfpath = selectedFile;
          elfdir = selectedDir;
          board.cpu.init();
          if(!board.cpu.loadElf(elfpath)) {
            disasms = board.cpu.disassembleAll();
            board.pause = false;
          }
        }
        ImGuiFileDialog::Instance()->Close();
      }
    }
    { /* Main Panel */
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
      DrawSplitter(false, 10, &wLC, &wR, 50, 50);
      wC = wLC - wL - thick;
      wB = wLC;
      ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(wLC, hR), ImGuiWindowFlags_NoTitleBar);
        DrawSplitter(true, 10, &hLC, &hB, 50, 50);
        ImGui::BeginChild(ImGui::GetID((void*)1), ImVec2(wLC, hLC), ImGuiWindowFlags_NoTitleBar);
          DrawSplitter(false, 10, &wL, &wC, 50, 50);
          ImGui::BeginChild(ImGui::GetID((void*)2), ImVec2(wL, hLC), ImGuiWindowFlags_NoTitleBar);
          {
            ImGui::Text("Control Panel\n");
            ImGui::Separator();
            ImGui::Text("Video\n");
            ImGui::Checkbox("BG0", &board.ppu.debug.enableBg[0].v1); ImGui::SameLine();
            ImGui::Checkbox("BG1", &board.ppu.debug.enableBg[1].v1); ImGui::SameLine();
            ImGui::Checkbox("BG2", &board.ppu.debug.enableBg[2].v1); ImGui::SameLine();
            ImGui::Checkbox("BG3", &board.ppu.debug.enableBg[3].v1);
            ImGui::Checkbox("SP ", &board.ppu.debug.enableSp.v1);
            ImGui::Separator();
            ImGui::Text("Audio\n");
            ImGui::Checkbox("Ch0", &board.apu.debug.enableCh[0 ].v1); ImGui::SameLine();
            ImGui::Checkbox("Ch1", &board.apu.debug.enableCh[1 ].v1); ImGui::SameLine();
            ImGui::Checkbox("Ch2", &board.apu.debug.enableCh[2 ].v1); ImGui::SameLine();
            ImGui::Checkbox("Ch3", &board.apu.debug.enableCh[3 ].v1);
            ImGui::Checkbox("Ch4", &board.apu.debug.enableCh[4 ].v1); ImGui::SameLine();
            ImGui::Checkbox("Ch5", &board.apu.debug.enableCh[5 ].v1); ImGui::SameLine();
            ImGui::Checkbox("Ch6", &board.apu.debug.enableCh[6 ].v1); ImGui::SameLine();
            ImGui::Checkbox("Ch7", &board.apu.debug.enableCh[7 ].v1);
            ImGui::Separator();
            ImGui::Text("Input\n");
            HwPad hwpad = board.io.getPadStatus();
            bool A = hwpad.A, B = hwpad.B, C = hwpad.C, D = hwpad.D;
            bool L = hwpad.L, R = hwpad.R, S = hwpad.S, T = hwpad.T;
            bool Up = hwpad.UP, Dw = hwpad.DOWN, Lf = hwpad.LEFT, Rt = hwpad.RIGHT;
            ImGui::Checkbox("A", &A); ImGui::SameLine();
            ImGui::Checkbox("B", &B); ImGui::SameLine();
            ImGui::Checkbox("C", &C); ImGui::SameLine();
            ImGui::Checkbox("D", &D);
            ImGui::Checkbox("L", &L); ImGui::SameLine();
            ImGui::Checkbox("R", &R); ImGui::SameLine();
            ImGui::Checkbox("S", &S); ImGui::SameLine();
            ImGui::Checkbox("T", &T);
            ImGui::Checkbox("Up", &Up); ImGui::SameLine();
            ImGui::Checkbox("Dw", &Dw); ImGui::SameLine();
            ImGui::Checkbox("Lf", &Lf); ImGui::SameLine();
            ImGui::Checkbox("Rt", &Rt);
          }
          ImGui::EndChild();
          ImGui::SameLine();
          ImGui::BeginChild(ImGui::GetID((void*)3), ImVec2(wC, hLC), ImGuiWindowFlags_NoTitleBar);
          {
            float gameW = wC;
            float gameH = 240.0 / 320 * gameW;
            ImGui::Image(reinterpret_cast<void *>(framebuffer), ImVec2(gameW, gameH));
          }
          ImGui::EndChild();
        ImGui::EndChild();
        ImGui::BeginChild(ImGui::GetID((void*)4), ImVec2(wB, hB), ImGuiWindowFlags_NoTitleBar);
        {
          ImGui::Text("Viewer Panel");
          for (auto& s: disasms) {
            ImGui::Text("%s", s.c_str());
          }
        }
        ImGui::EndChild();
      ImGui::EndChild();
      ImGui::SameLine();
      ImGui::BeginChild(ImGui::GetID((void*)5), ImVec2(wR, hR), ImGuiWindowFlags_NoTitleBar);
      {
        ImGui::Text("Property Panel\nHwREG\n");
      }
      ImGui::EndChild();
      ImGui::PopStyleVar(3);
    }
    ImGui::End();

    // auto imguiWindowFlag2 = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
    // auto pOpen2 = true;
    // ImGui::SetNextWindowPos(ImVec2(320, 240), ImGuiCond_FirstUseEver);
    // ImGui::SetNextWindowSize(ImVec2(320+20, 240+20));
    // ImGui::Begin("Another Window 2", &pOpen2, imguiWindowFlag2);
    // // ImGui::Image((void *)framebuffer, ImVec2(320, 240));
    // ImGui::Image(reinterpret_cast<void *>(framebuffer), ImVec2(320, 240));
    // ImGui::End();

    // Rendering
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGui::Render();

    {
      int width = 320, height = 240;
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
      glViewport(0, 0, 320, 240);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.data());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    }

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
  if (true) {
    glDeleteRenderbuffers(1, &renderbuffer);
    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &framebuffer);
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  // if (audioDev) {
  //   SDL_PauseAudioDevice(audioDev, true);
  //   SDL_CloseAudioDevice(audioDev);
  // }
  // if (window) {
  //   SDL_DestroyWindow(window);
  // }
  // SDL_Quit();

  return 0;
}

void DrawSplitter(int split_vertically, float thickness, float* size0, float* size1, float min_size0, float min_size1)
{
  ImVec2 backup_pos = ImGui::GetCursorPos();
  if (split_vertically)
      ImGui::SetCursorPosY(backup_pos.y + *size0);
  else
      ImGui::SetCursorPosX(backup_pos.x + *size0);

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
  // We don't draw while active/pressed because as we move the panes the splitter button will be 1 frame late
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0,0,0,0));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f,0.6f,0.6f,0.10f));
  ImGui::Button("##Splitter", ImVec2(!split_vertically ? thickness : -1.0f, split_vertically ? thickness : -1.0f));
  ImGui::PopStyleColor(3);

  ImGui::SetItemAllowOverlap(); // This is to allow having other buttons OVER our splitter. 

  if (ImGui::IsItemActive())
  {
    float mouse_delta = split_vertically ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;

    // Minimum pane size
    if (mouse_delta < min_size0 - *size0)
        mouse_delta = min_size0 - *size0;
    if (mouse_delta > *size1 - min_size1)
        mouse_delta = *size1 - min_size1;

    // Apply resize
    *size0 += mouse_delta;
    *size1 -= mouse_delta;
  }
  ImGui::SetCursorPos(backup_pos);
}
