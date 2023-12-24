#pragma once
#include <map>
#include <memory>

class Cpu;
class RegisterSet;
class Memory;

enum OpType {
  OPTYPE_R = 0,
  OPTYPE_I,
  OPTYPE_S,
  OPTYPE_U,
  OPTYPE_B,
  OPTYPE_J,
  OPTYPE_SYSTEM,
  OPTYPE_ZICSR,

  OPTYPE_CR,
  OPTYPE_CI,
  OPTYPE_CSS,
  OPTYPE_CIW,
  OPTYPE_CL,
  OPTYPE_CS,
  OPTYPE_CA,
  OPTYPE_CB2,  // c.andi, c.ori, ...
  OPTYPE_CB,
  OPTYPE_CJ,
};

enum InstructionIdI {
  INSTR_UNKNOWN = 0,
  INSTR_LUI,
  INSTR_AUIPC,
  INSTR_JAL,
  INSTR_JALR,
  INSTR_BEQ,
  INSTR_BNE,
  INSTR_BLT,
  INSTR_BGE,
  INSTR_BLTU,
  INSTR_BGEU,
  INSTR_LB,
  INSTR_LH,
  INSTR_LW,
  INSTR_LBU,
  INSTR_LHU,
  INSTR_SB,
  INSTR_SH,
  INSTR_SW,
  INSTR_ADDI,
  INSTR_SLTI,
  INSTR_SLTIU,
  INSTR_XORI,
  INSTR_ORI,
  INSTR_ANDI,
  INSTR_SLLI,
  INSTR_SRLI,
  INSTR_SRAI,
  INSTR_ADD,
  INSTR_SUB,
  INSTR_SLL,
  INSTR_SLT,
  INSTR_SLTU,
  INSTR_XOR,
  INSTR_SRL,
  INSTR_SRA,
  INSTR_OR,
  INSTR_AND,
  // INSTR_FENCE,
  INSTR_ECALL,
  INSTR_EBREAK,
  INSTR_MRET,
  INSTR_WFI,

  // INSTR_FENCEI,

  INSTR_CSRRW,
  INSTR_CSRRS,
  INSTR_CSRRC,
  INSTR_CSRRWI,
  INSTR_CSRRSI,
  INSTR_CSRRCI,

  INSTR_MUL,
  INSTR_MULH,
  INSTR_MULHSU,
  INSTR_MULHU,
  INSTR_DIV,
  INSTR_DIVU,
  INSTR_REM,
  INSTR_REMU,
};

enum InstructionIdC {
  INSTR_CUNKNOWN = 0,
  INSTR_CADDI4SPN,
  // INSTR_CFLD,
  // INSTR_CLQ,
  INSTR_CLW,
  // INSTR_CFLW,
  // INSTR_CLD,
  // INSTR_CFSD,
  // INSTR_CSQ,
  INSTR_CSW,
  // INSTR_CFSW,
  // INSTR_CSD,
  INSTR_CNOP,
  INSTR_CADDI,
  INSTR_CJAL,
  // INSTR_CADDIW,
  INSTR_CLI,
  INSTR_CADDI16SP,
  INSTR_CLUI,
  INSTR_CSRLI,
  // INSTR_CSRLI64,
  INSTR_CSRAI,
  // INSTR_CSRAI64,
  INSTR_CANDI,
  INSTR_CSUB,
  INSTR_CXOR,
  INSTR_COR,
  INSTR_CAND,
  // INSTR_CSUBW,
  // INSTR_CADDW,
  INSTR_CJ,
  INSTR_CBEQZ,
  INSTR_CBNEZ,

  INSTR_CSLLI,
  // INSTR_CSLLI64,
  // INSTR_CFLDSP,
  // INSTR_CLQSP,
  INSTR_CLWSP,
  // INSTR_CFLWSP,
  // INSTR_CLDSP,
  INSTR_CJR,
  INSTR_CMV,
  INSTR_CEBREAK,
  INSTR_CJALR,
  INSTR_CADD,
  // INSTR_CFSDSP,
  // INSTR_CSQSP,
  INSTR_CSWSP,
  // INSTR_CFSWSP,
  // INSTR_CSDSP,
};

enum InstructionOpcode {
  OPCODE_C0 = 0x0,
  OPCODE_C1 = 0x1,
  OPCODE_C2 = 0x2,
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
};

enum InstructionFunctI {
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
  FUNCT3_SLLI  = 1,
  FUNCT3_SRLI  = 5,  // imm[11:5]=0000000
  FUNCT3_SRAI  = 5,  // imm[11:5]=0100000
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

enum InstructionFunctC {
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
};

enum InstructionPhase {
  INSTR_PHASE_FETCH = 0,
  INSTR_PHASE_DECODE,
  INSTR_PHASE_EXECUTE,
  INSTR_PHASE_POSTPROC,
};

enum InstructionExceptionCause {
  EXCEPTION_INSTR_ADDR_MISALIGNED = 0,
  EXCEPTION_INSTR_ACCESS_FAULT,
  EXCEPTION_ILLEGAL_INSTR,
  EXCEPTION_BREAKPOINT,
  EXCEPTION_LOAD_ADDR_MISALIGNED,
  EXCEPTION_LOAD_ACCESS_FAULT,
  EXCEPTION_STORE_ADDR_MISALIGNED,
  EXCEPTION_STORE_ACCESS_FAULT,
  EXCEPTION_ENV_CALL_FROM_UMODE,
  EXCEPTION_ENV_CALL_FROM_SMODE,
  EXCEPTION_RESERVED_10,
  EXCEPTION_ENV_CALL_FROM_MMODE,
  EXCEPTION_INSTR_PAGE_FAULT,
  EXCEPTION_LOAD_PAGE_FAULT,
  EXCEPTION_RESERVED_14,
  EXCEPTION_STORE_PAGE_FAULT,
};

struct Instruction {
  uint32_t binary;
  uint32_t instr;
  uint8_t opcode;
  uint8_t dst;
  uint8_t src1;
  uint8_t src2;
  uint8_t src3;
  int8_t waitCycle;
  uint8_t phase;
  uint8_t pad2;
  union32_t imm;  // funct12
  union32_t result;
  int32_t size;
  OpType type;
  bool isJumped;
  bool isWaiting;
  Instruction() {}
  virtual ~Instruction() {}
};

class CpuIsaRV32I {
  void (CpuIsaRV32I::*execute_tableI[64])(Instruction& instr, RegisterSet& regs, Memory& memory);
  void (CpuIsaRV32I::*execute_tableC[64])(Instruction& instr, RegisterSet& regs, Memory& memory);
  map<uint32_t, Instruction> instrCache;
public:
  CpuIsaRV32I();
  ~CpuIsaRV32I();
  auto initI() -> void;
  auto initC() -> void;

  auto checkInterruption(Instruction& instr, RegisterSet& regs, Memory& memory) -> bool;
  auto handleInterruption(Instruction& instr, RegisterSet& regs) -> void;
  auto returnInterruption(Instruction& instr, RegisterSet& regs) -> void;
  auto setException(Instruction& instr, RegisterSet& regs, uint32_t cause) -> void;
  auto handleException(Instruction& instr, RegisterSet& regs) -> void;

  auto fetch(Instruction& instr) -> void;
  auto decode(uint32_t bytes, Instruction& instr) -> void;
  auto execute(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;

  auto decode32(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeR(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeI(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeS(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeB(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeU(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeJ(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeSystem(uint32_t bytes, Instruction& instr) -> void;
  auto decode16(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeCR(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeCI(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeCSS(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeCIW(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeCL(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeCS(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeCA(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeCB2(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeCB(uint32_t bytes, Instruction& instr) -> void;
  auto decodeTypeCJ(uint32_t bytes, Instruction& instr) -> void;

  auto execute_unknown(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_lui(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_auipc(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_jal(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_jalr(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_beq(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_bne(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_blt(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_bge(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_bltu(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_bgeu(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_lb(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_lh(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_lw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_lbu(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_lhu(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_sb(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_sh(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_sw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_addi(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_slti(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_sltiu(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_xori(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_ori(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_andi(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_slli(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_srli(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_srai(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_add(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_sub(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_sll(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_slt(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_sltu(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_xor(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_srl(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_sra(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_or(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_and(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // execute_fence(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_ecall(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_ebreak(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_mret(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_wfi(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // execute_fencei(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_csrrw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_csrrs(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_csrrc(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_csrrwi(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_csrrsi(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_csrrci(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_mul(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_mulh(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_mulhsu(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_mulhu(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_div(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_divu(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_rem(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_remu(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;

  auto execute_cunknown(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_caddi4spn(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cfld(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_clq(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_clw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cflw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cld(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cfsd(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_csq(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_csw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cfsw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_csd(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cnop(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_caddi(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cjal(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_caddiw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cli(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_caddi16sp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_clui(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_csrli(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_csrli64(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_csrai(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_csrai64(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_candi(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_csub(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cxor(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cor(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cand(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_csubw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_caddw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cj(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cbeqz(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cbnez(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;

  auto execute_cslli(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cslli64(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cfldsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_clqsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_clwsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cflwsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cldsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cjr(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cmv(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cebreak(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cjalr(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cadd(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cfsdsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_csqsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto execute_cswsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_cfswsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  // auto execute_csdsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void;

  auto instrToStr(int64_t icount, uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> const string;
  auto printInstr(int64_t icount, uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> void;
  auto disassemble(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeR(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeI(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeS(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeB(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeU(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeJ(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeSystem(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeCR(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeCI(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeCSS(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeCIW(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeCL(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeCS(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeCA(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeCB2(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeCB(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
  auto disassembleTypeCJ(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory) -> string;
};
