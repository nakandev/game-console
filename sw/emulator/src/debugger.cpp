#include <debugger.h>
#include <cpu.h>
#include <iostream>
#include <sstream>
#include <fmt/core.h>

static auto splitCommandString(const string& cmdstr) -> vector<string>
{
  char delim = ' ';
  vector<string> cmds;
  int begin = 0, end = 0;
  while (begin < cmdstr.size()) {
    for (end=begin; end<cmdstr.size(); ++end) {
      if (cmdstr[end] == delim) break;
    }
    auto cmd = cmdstr.substr(begin, end - begin);
    cmds.push_back(cmd);
    for (begin=end; begin<cmdstr.size(); ++begin) {
      if (cmdstr[begin] != delim) break;
    }
  }
  return std::move(cmds);
}

Debugger::Debugger()
: response(), commandStrings(),
  currentBreakPointId(), breakPoints(),
  cpu(nullptr)
{
}

Debugger::~Debugger()
{
  response = "";
  commandStrings.clear();
  currentBreakPointId = 0;
  breakPoints.clear();
  cpu = nullptr;
}

auto Debugger::setCpu(Cpu& cpu) -> void
{
  this->cpu = &cpu;
}

auto Debugger::runInterpreter() -> void
{
  bool done = false;
  while (!done) {
    input_command();
    int ret = exec_command();
    if (ret) {
      done = true;
    }
    disassemble_pc();
  }
}

auto Debugger::input_command() -> void
{
  const char delim = ' ';
  string cmd;
  fmt::print("> ");
  getline(cin, cmd);

  if (cmd.size() > 0) {
    commandStrings.clear();
    stringstream ss(cmd);
    string subs = "";
    while (getline(ss, subs, delim)) {
      commandStrings.push_back(subs);
    }
  }
}

auto Debugger::exec_command() -> int
{
  auto& cmd = commandStrings;
  if (cmd.size() == 0) {
    return 0;
  }
  string cmdname = cmd[0];
  if (cmdname == "q" || cmdname == "quit") {
    return 256;
  } else
  if (cmdname == "dis" || cmdname == "disassemble") {
    return disassemble_all();
  } else
  if (cmdname == "reg") {
    return register_read(0, 0, 0);
  } else
  if (cmdname == "b") {
    if (cmd.size() >= 2) {
      string label = cmd[1];
      return breakpoint_set(label);
    } else {
      return 0;
    }
  } else
  if (cmdname == "run") {
    return process_launch();
  } else
  if (cmdname == "pc") {
    return disassemble_pc();
  } else
  if (cmdname == "s" || cmdname == "si") {
    return step_in();
  }
  return 0;
}

auto Debugger::breakpoint_set(const string& label) -> int
{
  auto bp = BreakPoint{currentBreakPointId, -1, -1, "notitle"};
  if (label.starts_with("0x")) {
    uint32_t value;
    stringstream ss(label.substr(2));
    ss >> hex >> value;
    fmt::print("added breakpoint: pc=0x{:08x}\n", value);
    bp.pc = value;
    breakPoints.push_back(std::move(bp));
  }
  ++currentBreakPointId;
  return 0;
}

auto Debugger::step_in() -> int
{
  if (!cpu) return 1;
  cpu->stepCycle();
  return 0;
}

auto Debugger::disassemble_pc() -> int
{
  if (!cpu) return 1;
  fmt::print("pc:{:08x}\n", cpu->getPc());
  return 0;
}

auto Debugger::disassemble_all() -> int
{
  if (!cpu) return 1;
  cpu->disassembleAll();
  return 0;
}

auto Debugger::register_read(uint32_t group, uint32_t no, uint8_t fmt) -> int
{
  if (!cpu) return 1;
  auto regStrs = cpu->readRegisterAll();
  for (auto& rs: regStrs) {
    fmt::print("{}\n", rs);
  }
  return 0;
}

auto Debugger::process_launch() -> int
{
  if (!cpu) return 1;
  long long maxCycle = 10000000;
  for (long long i=0; i<maxCycle; i++) {
    bool _break = false;
    for (auto& bp: breakPoints) {
      auto pc = cpu->getPc();
      if (pc == bp.pc) {
        fmt::print("pc={:08x} bp={:08x}\n", pc, bp.pc);
        _break = true;
        break;
      }
    }
    if (_break) break;
    cpu->stepCycle();
  }
  return 0;
}
