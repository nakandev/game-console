#pragma once
#include <common.h>
#include <vector>
#include <string>

using namespace std;

class Register {
public:
  union32_t val;
  size_t no;
  string name;
  Register();
  Register(size_t no, string name);
  ~Register();
};

class RegisterGroup {
private:
  vector<Register> regs;  // sparce array of Register
public:
  RegisterGroup();
  RegisterGroup(size_t size);
  ~RegisterGroup();
  inline auto operator[](size_t index) noexcept -> Register& { return regs[index]; }
  auto insert(size_t index, const Register& reg) -> void;
  auto size() -> size_t { return regs.size(); }
  auto init() -> void;
  auto dump() -> void;
};

class SparceRegisterGroup {
private:
  vector<Register> regs;  // sparce array of Register
  vector<size_t> indexs;
  auto at(size_t index) -> Register&;
public:
  SparceRegisterGroup();
  SparceRegisterGroup(size_t size);
  ~SparceRegisterGroup();
  inline auto operator[](size_t index) noexcept -> Register& {return at(index); }
  auto insert(size_t index, const Register& reg) -> void;
  auto size() -> size_t { return regs.size(); }
  auto init() -> void;
  auto dump() -> void;
};

class RegisterSet {
public:
  Register pc;
  Register prev_pc;
  RegisterGroup gpr;
  SparceRegisterGroup csr;
  RegisterSet();
  ~RegisterSet();
  auto init() -> void;
};

enum {
  MVENDORID    = 0x0F11,
  MARCHID      = 0x0F12,
  MIMPID       = 0x0F13,
  MHARTID      = 0x0F14,
  MCONFIGPTR   = 0x0F15,

  MSTATUS      = 0x0300,
  MISA         = 0x0301,
  MEDELEG      = 0x0302,
  MIDELEG      = 0x0303,
  MIE          = 0x0304,
  MTVEC        = 0x0305,
  MCOUNTEREN   = 0x0306,
  MSTATUSH     = 0x0310,

  MSCRATCH     = 0x0340,
  MEPC         = 0x0341,
  MCAUSE       = 0x0342,
  MTVAL        = 0x0343,
  MIP          = 0x0344,
  MTINST       = 0x034A,
  MTVAL2       = 0x034B,

  MENVCFG      = 0x030A,
  MENVCFGH     = 0x031A,
  MSECCFG      = 0x0747,
  MSECCFGH     = 0x0757,
};
