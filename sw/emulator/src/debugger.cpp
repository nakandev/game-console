#include <debugger.h>
#include <cpu.h>
#include <iostream>
#include <sstream>
#include <fmt/core.h>

Debugger::Debugger()
: commandStrings(), breakPoints(),
  cpu(nullptr)
{
}

Debugger::~Debugger()
{
  commandStrings.clear();
  breakPoints.clear();
  cpu = nullptr;
}

void
Debugger::setCpu(Cpu& cpu)
{
  this->cpu = &cpu;
}

void
Debugger::runInterpreter()
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

void
Debugger::input_command()
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

int
Debugger::exec_command()
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

int
Debugger::breakpoint_set(const string& label)
{
  if (label.starts_with("0x")) {
    uint32_t point;
    stringstream ss(label.substr(2));
    ss >> hex >> point;
    breakPoints.push_back(point);
    fmt::print("added breakpoint: pc=0x{:08x}\n", point);
  }
  return 0;
}

int
Debugger::step_in()
{
  if (!cpu) return 1;
  cpu->stepCycle();
  return 0;
}

int
Debugger::disassemble_pc()
{
  if (!cpu) return 1;
  cpu->printPc();
  return 0;
}

int
Debugger::disassemble_all()
{
  if (!cpu) return 1;
  cpu->disassembleAll();
  return 0;
}

int
Debugger::register_read(uint32_t group, uint32_t no, uint8_t fmt)
{
  if (!cpu) return 1;
  auto regStrs = cpu->readRegisterAll();
  for (auto& rs: regStrs) {
    fmt::print("{}\n", rs);
  }
  return 0;
}

int
Debugger::process_launch()
{
  if (!cpu) return 1;
  int maxCycle = 5000;
  for (int i=0; i<maxCycle; i++) {
    bool _break = false;
    for (auto& bp: breakPoints) {
      auto pc = cpu->getPc();
      // fmt::print("pc={:08x} bp={:08x}\n", pc, bp);
      if (pc == bp) {
        _break = true;
        break;
      }
    }
    if (_break) break;
    cpu->stepCycle();
  }
  return 0;
}
