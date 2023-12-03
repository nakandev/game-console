from processordesign import OpCode
from processordesign import instruction, opcode, assembly, binary
# from processordesign import regs, RegisterGroup, Register
from processordesign import reg_read, reg_write, imm_load, mem_read, mem_write

# Base Instruction
opc_alui = (
    OpCode('addi'),
    OpCode('slti'),
    OpCode('sltiu'),
    OpCode('andi'),
    OpCode('ori'),
    OpCode('xori'),
)

@instruction
def alui():
    opcode(opc_alui)
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def body():
        if ins.opc == 'addi':
            result = reg_read(ins.rs1) + imm_load(ins.imm)
        elif ins.opc == 'slti':
            result = reg_read(ins.rs1) < imm_load(ins.imm)
        elif ins.opc == 'sltiu':
            result = reg_read(ins.rs1) < imm_load(ins.imm)
        elif ins.opc == 'andi':
            result = reg_read(ins.rs1) & imm_load(ins.imm)
        elif ins.opc == 'ori':
            result = reg_read(ins.rs1) | imm_load(ins.imm)
        elif ins.opc == 'xori':
            result = reg_read(ins.rs1) ^ imm_load(ins.imm)
        else:
            result = 0
        reg_write(ins.dst, result)


opc_slli = ('slli', ),
opc_srli = ('srli', ),
opc_srai = ('srai', ),

opc_lui = ('lui', ),
opc_auipc = ('auipc', ),

opc_alu = (
    OpCode('add'),
    OpCode('slt'),
    OpCode('sltu'),
    OpCode('and'),
    OpCode('or'),
    OpCode('xor'),
    OpCode('sll'),
    OpCode('srl'),
    OpCode('sub'),
    OpCode('sra'),
)

@instruction
def alu():
    opcode(opc_alu)
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def body():
        if ins.opc == 'add':
            result = reg_read(ins.rs1) + reg_read(ins.rs2)
        elif ins.opc == 'slt':
            result = reg_read(ins.rs1) < reg_read(ins.rs2)
        elif ins.opc == 'sltu':
            result = reg_read(ins.rs1) < reg_read(ins.rs2)
        elif ins.opc == 'and':
            result = reg_read(ins.rs1) & reg_read(ins.rs2)
        elif ins.opc == 'or':
            result = reg_read(ins.rs1) | reg_read(ins.rs2)
        elif ins.opc == 'xor':
            result = reg_read(ins.rs1) ^ reg_read(ins.rs2)
        elif ins.opc == 'sll':
            result = reg_read(ins.rs1) << reg_read(ins.rs2)
        elif ins.opc == 'srl':
            result = reg_read(ins.rs1) >> reg_read(ins.rs2)
        elif ins.opc == 'sub':
            result = reg_read(ins.rs1) - reg_read(ins.rs2)
        elif ins.opc == 'sra':
            result = reg_read(ins.rs1) >> reg_read(ins.rs2)
        else:
            result = 0
        reg_write(ins.dst, result)


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

opc_load = (
    OpCode('lw'),
    OpCode('lh'),
    OpCode('lb'),
)
opc_store = (
    OpCode('sw'),
    OpCode('sh'),
    OpCode('sb'),
)

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


@instruction
def load():
    opcode(opc_load)
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def body():
        addr = reg_read(ins.rs1)
        offset = imm_load(ins.imm)
        if ins.opc == 'lw':
            size = 4
        elif ins.opc == 'lh':
            size = 2
        else:  # 'lb'
            size = 1
        result = mem_read(addr + offset, size)
        reg_write(ins.dst, result)


@instruction
def store():
    opcode(opc_store)
    ins = assembly('$opc:OpCode $rd:GPR, $imm:Imm ($rs1:GPR)')
    binary(ins.imm[11:0], ins.rs1, ins.opc[9:7], ins.rd, ins.opc[6:0])

    def body():
        addr = reg_read(ins.rs1)
        offset = imm_load(ins.imm)
        if ins.opc == 'sw':
            size = 4
        elif ins.opc == 'sh':
            size = 2
        else:  # 'sb'
            size = 1
        result = mem_read(addr + offset, size)
        mem_write(ins.dst, result)
