#pragma once

#include <common.h>
#include <memorymap.h>
#include <string>
#include <vector>

using namespace std;

class Cpu;

struct BreakPoint {
  uint32_t id;
  int64_t pc;
  int64_t line;
  string name;
};

class Debugger {
  string response;
  vector<string> commandStrings;
  uint32_t currentBreakPointId;
  vector<BreakPoint> breakPoints;
  Cpu* cpu;
public:
  Debugger();
  ~Debugger();
  auto setCpu(Cpu& cpu) -> void;
  auto runInterpreter() -> void;
  auto input_command() -> void;
  auto exec_command() -> int;
  auto breakpoint_set(const string& label) -> int;
  auto breakpoint_delete(const uint32_t id) -> int;
  auto breakpoint_enable() -> int;
  auto breakpoint_disable() -> int;
  auto watchpoint_set(const string& label) -> int;
  auto watchpoint_delete(const uint32_t id) -> int;
  auto watchpoint_enable() -> int;
  auto watchpoint_disable() -> int;
  auto step_in() -> int;
  auto step_over() -> int;
  auto step_out() -> int;
  auto disassemble_pc() -> int;
  auto disassemble_func() -> int;
  auto disassemble_all() -> int;
  auto register_read(uint32_t group, uint32_t no, uint8_t fmt) -> int;
  auto register_write(uint32_t group, uint32_t no, uint32_t value) -> int;
  auto memory_read(uint32_t addr, uint32_t size, uint32_t count, uint8_t fmt) -> int;
  auto memory_write(uint32_t addr, uint32_t size, vector<uint32_t>& values) -> int;
  auto process_launch() -> int;
  auto process_continue() -> int;
};
