#include <debugger.h>
#include <cpu.h>
#include <iostream>
#include <sstream>
#include <fmt/core.h>

Debugger::Debugger()
  : cpu(nullptr)
{
}

Debugger::~Debugger()
{
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
  if (cmdname == "pc") {
    return disassemble_pc();
  } else
  if (cmdname == "s") {
    return step_in();
  }
  return 0;
}

int
Debugger::step_in()
{
  if (!cpu) return 1;
  cpu->stepi();
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
