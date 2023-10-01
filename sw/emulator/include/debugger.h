#pragma once

#include <common.h>
#include <memorymap.h>
#include <string>
#include <vector>

using namespace std;

class Cpu;

class Debugger {
  vector<string> commandStrings;
  vector<uint32_t> breakPoints;
  Cpu* cpu;
public:
  Debugger();
  ~Debugger();
  void setCpu(Cpu& cpu);
  void runInterpreter();
  void input_command();
  int exec_command();
  int breakpoint_set(const string& label);
  int breakpoint_delete(const uint32_t id);
  int breakpoint_enable();
  int breakpoint_disable();
  int watchpoint_set(const string& label);
  int watchpoint_delete(const uint32_t id);
  int watchpoint_enable();
  int watchpoint_disable();
  int step_in();
  int step_over();
  int step_out();
  int disassemble_pc();
  int disassemble_func();
  int disassemble_all();
  int register_read(uint32_t group, uint32_t no, uint8_t fmt);
  int register_write(uint32_t group, uint32_t no, uint32_t value);
  int memory_read(uint32_t addr, uint32_t size, uint32_t count, uint8_t fmt);
  int memory_write(uint32_t addr, uint32_t size, vector<uint32_t>& values);
  int process_launch();
  int process_continue();
};
