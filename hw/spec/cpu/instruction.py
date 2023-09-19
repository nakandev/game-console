from processordesign import OpCode
from processordesign import instruction, opcode, assembly, binary
from processordesign import reg_read, reg_write, imm_load, mem_read, mem_write


opc_load = (
    OpCode('lw', 0x1),
    OpCode('lh', ),
    OpCode('lb', ),
)
opc_store = (
    OpCode('sw', ),
    OpCode('sh', ),
    OpCode('sb', ),
)


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
        else:  # SB
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
        else:  # SB
            size = 1
        result = mem_read(addr + offset, size)
        mem_write(ins.dst, result)
