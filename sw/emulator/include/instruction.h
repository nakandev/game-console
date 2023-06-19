#pragma once
#include <memory>

class Cpu;
class RegisterSet;
class Memory;

typedef enum {
  OPTYPE_R = 0,
  OPTYPE_I,
  OPTYPE_S,
  OPTYPE_U,
  OPTYPE_B,
  OPTYPE_J,
  OPTYPE_SYSTEM,
  OPTYPE_ZICSR,

  OPTYPE_CR = 0,
  OPTYPE_CI,
  OPTYPE_CSS,
  OPTYPE_CIW,
  OPTYPE_CL,
  OPTYPE_CS,
  OPTYPE_CA,
  OPTYPE_CB,
  OPTYPE_CJ,
} OpType;

enum {
  OPCODE_LUI    = (0x0D << 2) | 3,
  OPCODE_AUIPC  = (0x05 << 2) | 3,
  OPCODE_JAL    = (0x1B << 2) | 3,
  OPCODE_JALR   = (0x19 << 2) | 3,
  OPCODE_BRANCH = (0x18 << 2) | 3,
  OPCODE_LOAD   = (0x00 << 2) | 3,
  OPCODE_STORE  = (0x08 << 2) | 3,
  OPCODE_ArithI = (0x04 << 2) | 3,
  OPCODE_Arith  = (0x0C << 2) | 3,
  OPCODE_FENCE  = (0x03 << 2) | 3,
  OPCODE_SYSTEM = (0x1C << 2) | 3,  // ECALL, EBREAK, ZiCSR
  FUNCT3_JALR  = 0,
  FUNCT3_BEQ   = 0,
  FUNCT3_BNE   = 1,
  FUNCT3_BLT   = 4,
  FUNCT3_BGE   = 5,
  FUNCT3_BLTU  = 6,
  FUNCT3_BGEU  = 7,
  FUNCT3_LB = 0,
  FUNCT3_LH = 1,
  FUNCT3_LW = 2,
  FUNCT3_LBU = 4,
  FUNCT3_LHU = 5,
  FUNCT3_SB = 0,
  FUNCT3_SH = 1,
  FUNCT3_SW = 2,
  FUNCT3_ADDI  = 0,
  FUNCT3_SLTI  = 2,
  FUNCT3_SLTIU = 3,
  FUNCT3_XORI  = 4,
  FUNCT3_ORI   = 6,
  FUNCT3_ANDI  = 7,
  FUNCT3_ADD  = 0,
  FUNCT3_SUB  = 0,  // func7=1
  FUNCT3_SLL  = 1,
  FUNCT3_SLT  = 2,
  FUNCT3_SLTU = 3,
  FUNCT3_XOR  = 4,
  FUNCT3_SRL  = 5,
  FUNCT3_SRA  = 5,  // funct7=1
  FUNCT3_OR   = 6,
  FUNCT3_AND  = 7,
  FUNCT3_FENCE = 0,
  FUNCT3_PRIV = 0,
  FUNCT12_ECALL  = 0x000,
  FUNCT12_EBREAK = 0x001,

  FUNCT12_MRET = 0x302,
  FUNCT12_WFI = 0x103,

  FUNCT3_MUL = 0,
  FUNCT3_MULH = 1,
  FUNCT3_MULHSU = 2,
  FUNCT3_MULHU = 3,
  FUNCT3_DIV = 4,
  FUNCT3_DIVU = 5,
  FUNCT3_REM = 6,
  FUNCT3_REMU = 7,
};

enum {
  OPCODE_C0 = 0x0,
  OPCODE_C1 = 0x1,
  OPCODE_C2 = 0x2,
  // TypeCR
  FUNCT4_C_JR   = 0x04,
  FUNCT4_C_JALR = 0x04,
  FUNCT3_C_MV   = 0x04,
  FUNCT3_C_ADD  = 0x04,
  // TypeCI
  FUNCT3_C_LWSP = 0x02,
  FUNCT3_C_LDSP = 0x03,
  // FUNCT3_C_LQSP,
  FUNCT3_C_FLWSP = 0x03,
  FUNCT3_C_FLDSP = 0x01,
  FUNCT3_C_LI       = 0x02,
  FUNCT3_C_LUI      = 0x03,
  FUNCT3_C_ADDI     = 0x00,
  FUNCT3_C_ADDIW = 0x01,
  FUNCT3_C_ADDI16SP = 0x03,
  FUNCT3_C_SLLI     = 0x00,
  FUNCT3_C_NOP      = 0x00,  // same as C_ADD
  FUNCT3_C_EBREAK   = 0x04,
  // TypeCSS
  FUNCT3_C_SWSP  = 0x06,
  FUNCT3_C_SDSP  = 0x07,
  // FUNCT3_C_SQSP,
  FUNCT3_C_FSWSP = 0x07,
  FUNCT3_C_FSDSP = 0x05,
  // TypeCIW
  FUNCT3_C_ADDI4SPN = 0x00,
  // TypeCL
  FUNCT3_C_LW  = 0x02,
  FUNCT3_C_LD  = 0x03,
  // FUNCT3_C_LQ,
  FUNCT3_C_FLW = 0x03,
  FUNCT3_C_FLD = 0x01,
  // TypeCS
  FUNCT3_C_SW  = 0x06,
  FUNCT3_C_SD  = 0x07,
  // FUNCT3_C_SQ,
  FUNCT3_C_FSW = 0x07,
  FUNCT3_C_FSD = 0x05,
  // TypeCA
  FUNCT6_C_AND  = (0x04<<3)|3, FUNCT2_C_AND   = 0x3,
  FUNCT6_C_OR   = (0x04<<3)|3, FUNCT2_C_OR    = 0x2,
  FUNCT6_C_XOR  = (0x04<<3)|3, FUNCT2_C_XOR   = 0x1,
  FUNCT6_C_SUB  = (0x04<<3)|3, FUNCT2_C_SUB   = 0x0,
  FUNCT6_C_ADDW = (0x04<<3)|7, FUNCT2_C_ADDW  = 0x0,
  FUNCT6_C_SUBW = (0x04<<3)|7, FUNCT2_C_SUBW  = 0x1,
  // TypeCB
  FUNCT3_C_BEQZ = 0x06,
  FUNCT3_C_BNEZ = 0x07,
  FUNCT3_C_SRLI = 0x04, FUNCT2_C_SRLI = 0x0,
  FUNCT3_C_SRAI = 0x04, FUNCT2_C_SRAI = 0x1,
  FUNCT3_C_ANDI = 0x00,
  // TypeCJ
  FUNCT3_C_J   = 0x05,
  FUNCT3_C_JAL = 0x01,

  FUNCT3_,
};

struct Instruction {
  uint32_t binary;
  uint8_t opcode;
  uint8_t funct3;
  uint8_t funct7;
  uint8_t dst;
  uint8_t src1;
  uint8_t src2;
  union32_t imm;
  union32_t result;
  int size;
  OpType type;
  bool isJumped;
  bool isWaiting;
};

class InstrManipulator {
public:
  InstrManipulator();
  ~InstrManipulator();
  virtual uint32_t fetch(uint32_t bytes) = 0;
  virtual Instruction decode(uint32_t bytes) = 0;
  virtual void execute(Instruction& instr, RegisterSet& regs, Memory& memory) = 0;
  virtual void writeback(Instruction& instr, RegisterSet& regs, Memory& memory) = 0;
};

class InstrRV32IManipulator {
public:
  InstrRV32IManipulator();
  ~InstrRV32IManipulator();
  void decode(uint32_t bytes, Instruction& instr);
  void execute(Instruction& instr, RegisterSet& regs, Memory& memory);
  void decodeTypeR(uint32_t bytes, Instruction& instr);
  void decodeTypeI(uint32_t bytes, Instruction& instr);
  void decodeTypeS(uint32_t bytes, Instruction& instr);
  void decodeTypeB(uint32_t bytes, Instruction& instr);
  void decodeTypeU(uint32_t bytes, Instruction& instr);
  void decodeTypeJ(uint32_t bytes, Instruction& instr);
  void decodeTypeSystem(uint32_t bytes, Instruction& instr);
  void decodeTypeExtC(uint32_t bytes, Instruction& instr);
  void decodeTypeCR(uint32_t bytes, Instruction& instr);
  void decodeTypeCI(uint32_t bytes, Instruction& instr);
  void decodeTypeCSS(uint32_t bytes, Instruction& instr);
  void decodeTypeCIW(uint32_t bytes, Instruction& instr);
  void decodeTypeCL(uint32_t bytes, Instruction& instr);
  void decodeTypeCS(uint32_t bytes, Instruction& instr);
  void decodeTypeCA(uint32_t bytes, Instruction& instr);
  void decodeTypeCB(uint32_t bytes, Instruction& instr);
  void decodeTypeCJ(uint32_t bytes, Instruction& instr);
  void executeTypeR(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeI(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeS(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeB(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeU(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeJ(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeSystem(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeExtC(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeCR(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeCI(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeCSS(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeCIW(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeCL(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeCS(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeCA(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeCB(Instruction& instr, RegisterSet& regs, Memory& memory);
  void executeTypeCJ(Instruction& instr, RegisterSet& regs, Memory& memory);
  void printInstrInfo(const Instruction& instr, const RegisterSet& regs, const Memory& memory);
  bool checkInterruption(Instruction& instr, RegisterSet& regs, Memory& memory);
  void handleInterruption(Instruction& instr, RegisterSet& regs);
  void returnInterruption(Instruction& instr, RegisterSet& regs);
};
