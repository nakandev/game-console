#include <emulator.h>
#include <debugger.h>
#include <chrono>
#include <thread>
#include <fmt/core.h>

using namespace std::chrono;

int main_disassembler(int argc, char* argv[])
{
  string path = "/home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/trial_02_tileram/trial_02_tileram";
  if (argc == 2) {
    path = string(argv[1]);
  }
  auto board = Board();
  board.cpu.loadElf(path);
  auto debugger = Debugger();
  debugger.setCpu(board.cpu);
  // debugger.disassemble_all();
  debugger.runInterpreter();
  return 0;
}

int main_emulator(int argc, char* argv[])
{
  using Duration = microseconds;
  vector<uint64_t> times;
  string path = "/home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/trial_02_tileram/trial_02_tileram";
  if (argc == 2) {
    path = string(argv[1]);
  }
  auto board = Board();

  debugLevel = 1;
  board.cpu.setMaxCycles(5000);
  board.cpu.loadElf(path);

  using namespace std;
  using namespace std::chrono;
  using dsec = duration<double>;
  auto msecPerFrame = microseconds(16666);
  for (auto i = 0; i < 20*60; ++i)
  {
    auto beginTime = high_resolution_clock::now();

    board.updateFrame();

    auto endTime = high_resolution_clock::now();
    auto procTime = duration_cast<microseconds>(endTime - beginTime);
    times.push_back(procTime.count());
    this_thread::sleep_until(beginTime + msecPerFrame);
  }

  // int i0 = 60;
  // for (int i=i0+0; i<times.size() && i<i0+100; i++) {
  //   float per = (float)times[i] / msecPerFrame.count();
  //   fmt::print("{:3d} {} {} {}\n", i, times[i], msecPerFrame.count(), per);
  // }

  return 0;
}

int main(int argc, char* argv[])
{
  // main_emulator(argc, argv);
  main_disassembler(argc, argv);
  return 0;
}
