#include <emulator.h>
#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{
  string path = "/home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/trial_02_tileram/trial_02_tileram";
  if (argc == 2) {
    path = string(argv[1]);
  }
  auto board = Board();

  debugLevel = 1;
  board.cpu.setMaxCycles(300);
  board.cpu.loadElf(path);
  board.cpu.initRegs();
  board.cpu.setEntrypoint(0x0800'0000);
  // board.cpu.launch(0x0800'0000);

  using namespace std;
  using namespace std::chrono;
  using dsec = duration<double>;
  auto t0 = steady_clock::now() + 16ms;
  for (auto i = 0; i < 10*60; ++i)
  {
    this_thread::sleep_until(t0);
    t0 += 16ms;
    board.updateFrameUntilVblank();
    board.updateFrameSinceVblank();
  }


  return 0;
}

