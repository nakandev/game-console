#pragma once

#include <common.h>
#include <memorymap.h>
#include <string>
#include <vector>

using namespace std;

class Cpu;

class Debugger {
  Cpu* cpu;
public:
  Debugger();
  ~Debugger();
  void setCpu(Cpu& cpu);
  void exec_command(const string& cmd);
  void breakpoint_set(const string& label);
  void breakpoint_delete(const uint32_t id);
  void breakpoint_enable();
  void breakpoint_disable();
  void watchpoint_set(const string& label);
  void watchpoint_delete(const uint32_t id);
  void watchpoint_enable();
  void watchpoint_disable();
  void step_in();
  void step_over();
  void step_out();
  void disassemble_pc();
  void disassemble_func();
  void disassemble_all();
  void register_read(uint32_t group, uint32_t no, uint8_t fmt);
  void register_write(uint32_t group, uint32_t no, uint32_t value);
  void memory_read(uint32_t addr, uint32_t size, uint32_t count, uint8_t fmt);
  void memory_write(uint32_t addr, uint32_t size, vector<uint32_t>& values);
  void process_launch();
  void process_continue();
};
