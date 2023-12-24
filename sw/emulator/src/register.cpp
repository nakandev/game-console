#include <register.h>
#include <fmt/core.h>

static const char* gprNames[] = {
  "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
};

Register::Register()
  : val(), no(), name()
{
}

Register::Register(size_t no, string name)
  : val(), no(no), name(name)
{
}

Register::~Register()
{
}

RegisterGroup::RegisterGroup()
  : regs()
{
}

RegisterGroup::~RegisterGroup()
{
  regs.clear();
}

auto RegisterGroup::insert(size_t index, const Register& reg) -> void
{
  if (regs.size() < index + 1) {
    regs.resize(index + 1);
  }
  regs[index] = reg;
}

auto RegisterGroup::init() -> void
{
  for (int i=0; i<regs.size(); i++) {
    regs[i].val.s = 0;
  }
}

auto RegisterGroup::dump() -> void
{
  for (auto& regitem: regs) {
    auto& reg = regitem;
    fmt::print("  {0:2d}:{1:4s} = {2:08x} ({2:d})\n", reg.no, reg.name, reg.val.s);
  }
}

SparceRegisterGroup::SparceRegisterGroup()
  : regs(), indexs()
{
}

SparceRegisterGroup::~SparceRegisterGroup()
{
  indexs.clear();
  regs.clear();
}

auto SparceRegisterGroup::insert(size_t index, const Register& reg) -> void
{
  // regs.insert(make_pair(index, reg));
  indexs.push_back(index);
  regs.push_back(reg);
}

auto SparceRegisterGroup::init() -> void
{
  for (int i=0; i<indexs.size(); i++) {
    regs[i].val.s = 0;
  }
}

auto SparceRegisterGroup::at(size_t index) -> Register&
{
  for (int i=0; i<indexs.size(); i++) {
    if (indexs[i] == index) {
      return regs[i];
    }
  }
  return regs[0];
}

auto SparceRegisterGroup::dump() -> void
{
  for (auto& regitem: regs) {
    // auto& reg = regitem.second;
    auto& reg = regitem;
    fmt::print("  {0:2d}:{1:4s} = {2:08x} ({2:d})\n", reg.no, reg.name, reg.val.s);
  }
}

RegisterSet::RegisterSet()
  : pc(), prev_pc(),
    gpr(RegisterGroup()),
    csr(SparceRegisterGroup())
{
  pc = Register(32, "pc");
  for (size_t i=0; i<32; i++) {
    gpr.insert(i, Register(i, gprNames[i]));
  }

  csr.insert(MVENDORID , Register(MVENDORID , "mvendorid"));
  csr.insert(MARCHID   , Register(MARCHID   , "marchid"));
  csr.insert(MIMPID    , Register(MIMPID    , "mimpid"));
  csr.insert(MHARTID   , Register(MHARTID   , "mhartid"));
  csr.insert(MCONFIGPTR, Register(MCONFIGPTR, "mconfigptr"));

  csr.insert(MSTATUS   , Register(MSTATUS   , "mstatus"));
  csr.insert(MISA      , Register(MISA      , "misa"));
  csr.insert(MEDELEG   , Register(MEDELEG   , "medeleg"));
  csr.insert(MIDELEG   , Register(MIDELEG   , "mideleg"));
  csr.insert(MIE       , Register(MIE       , "mie"));
  csr.insert(MTVEC     , Register(MTVEC     , "mtvec"));
  csr.insert(MCOUNTEREN, Register(MCOUNTEREN, "mcounteren"));
  csr.insert(MSTATUSH  , Register(MSTATUSH  , "mstatush"));

  csr.insert(MSCRATCH  , Register(MSCRATCH  , "mscratch"));
  csr.insert(MEPC      , Register(MEPC      , "mepc"));
  csr.insert(MCAUSE    , Register(MCAUSE    , "mcause"));
  csr.insert(MTVAL     , Register(MTVAL     , "mtval"));
  csr.insert(MIP       , Register(MIP       , "mip"));
  csr.insert(MTINST    , Register(MTINST    , "mtinst"));
  csr.insert(MTVAL2    , Register(MTVAL2    , "mtval2"));

  csr.insert(MENVCFG   , Register(MENVCFG   , "menvcfg"));
  csr.insert(MENVCFGH  , Register(MENVCFGH  , "menvcfgh"));
  csr.insert(MSECCFG   , Register(MSECCFG   , "mseccfg"));
  csr.insert(MSECCFGH  , Register(MSECCFGH  , "mseccfgh"));
}

RegisterSet::~RegisterSet()
{
}

auto RegisterSet::init() -> void
{
  pc.val.s = 0;
  prev_pc.val.s = 0;
  gpr.init();
  csr.init();
}
