from processordesign import instruction, assembly, binary
from processordesign import ins, rf, mem, sext


# Base Instruction
class addi(instruction):
    assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = rf.gpr[ins.rs1] + sext(ins.imm)

class slti(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = rf.gpr[ins.rs1] < sext(ins.imm)

class sltiu(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = rf.gpr[ins.rs1].u < sext(ins.imm)

class andi(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = rf.gpr[ins.rs1] & sext(ins.imm)

class ori(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = rf.gpr[ins.rs1] | sext(ins.imm)

class xori(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = rf.gpr[ins.rs1] ^ sext(ins.imm)

class slli(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])  # ?

    def implement():
        rf.gpr[ins.rd] = rf.gpr[ins.rs1] << ins.imm

class srli(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])  # ?

    def implement():
        rf.gpr[ins.rd] = rf.gpr[ins.rs1].u >> ins.imm

class srai(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])  # ?

    def implement():
        rf.gpr[ins.rd] = rf.gpr[ins.rs1] >> ins.imm


opc_lui = ('lui', ),
opc_auipc = ('auipc', ),

opc_add = ('add', ),
opc_slt = ('slt', ),
opc_sltu = ('sltu', ),
opc_and = ('and', ),
opc_or = ('or', ),
opc_xor = ('xor', ),
opc_sll = ('sll', ),
opc_srl = ('srl', ),
opc_sub = ('sub', ),
opc_sra = ('sra', ),

opc_nop = ('nop', ),  # alias

opc_jal = ('jal', ),
opc_jalr = ('jalr', ),
opc_beq = ('beq', ),
opc_bne = ('bne', ),
opc_blt = ('blt', ),
opc_bltu = ('bltu', ),
opc_bgt = ('bgt', ),
opc_bgtu = ('bgtu', ),
opc_bge = ('bge', ),
opc_bgeu = ('bgeu', ),

class lb(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = sext(mem[rf.gpr[ins.rs1] + ins.imm][7:0])

class lh(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = sext(mem[rf.gpr[ins.rs1] + ins.imm][15:0])

class lw(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = sext(mem[rf.gpr[ins.rs1] + ins.imm][31:0])

class lbu(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = mem[rf.gpr[ins.rs1] + ins.imm][7:0]

class lhu(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = mem[rf.gpr[ins.rs1] + ins.imm][15:0]

class lwu(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        rf.gpr[ins.rd] = mem[rf.gpr[ins.rs1] + ins.imm][31:0]

class sb(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        mem[rf.gpr[ins.rs1] + ins.imm][7:0] = rf.gpr[ins.dst]

class sh(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        mem[rf.gpr[ins.rs1] + ins.imm][15:0] = rf.gpr[ins.dst]

class sw(instruction):
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def implement():
        mem[rf.gpr[ins.rs1] + ins.imm][31:0] = rf.gpr[ins.dst]


opc_fence = ('fence', ),

opc_ecall = ('ecall', ),
opc_ebreak = ('ebreak', ),

# Zifencei
opc_fence_i = ('fence.i', ),

# M
opc_mul = ('mul', ),
opc_mulh = ('mulh', ),
opc_mulhs = ('mulhs', ),
opc_humhsu = ('mulhsu', ),

opc_div = ('div', ),
opc_divu = ('divu', ),
opc_rem = ('rem', ),
opc_remu = ('remu', ),

# Zicsr
opc_csrrw = ('csrrw', ),
opc_csrrs = ('csrrs', ),
opc_csrrc = ('csrrc', ),
opc_csrrwi = ('csrrwi', ),
opc_csrrsi = ('csrrsi', ),
opc_csrrci = ('csrrci', ),
