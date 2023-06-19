import random

code_template = '''
random_op:
  .global random_op
  .align 4
  addi sp, sp, -64;
  sw x1,   0(sp); sw x5,   4(sp); sw x6,   8(sp); sw x7,  12(sp);
  sw x10, 16(sp); sw x11, 20(sp); sw x12, 24(sp); sw x13, 28(sp);
  sw x14, 32(sp); sw x15, 36(sp); sw x16, 40(sp); sw x17, 44(sp);
  sw x28, 48(sp); sw x29, 52(sp); sw x30, 56(sp); sw x31, 60(sp);

{}

  lw x1,   0(sp); lw x5,   4(sp); lw x6,   8(sp); lw x7,  12(sp);
  lw x10, 16(sp); lw x11, 20(sp); lw x12, 24(sp); lw x13, 28(sp);
  lw x14, 32(sp); lw x15, 36(sp); lw x16, 40(sp); lw x17, 44(sp);
  lw x28, 48(sp); lw x29, 52(sp); lw x30, 56(sp); lw x31, 60(sp);
  addi sp, sp, 64;
  ret;
'''

typeRop = ('add', 'sub', 'or', 'and', 'xor')
typeIop = ('addi', 'ori', 'andi', 'xori')


def random_op():
    ops = []
    for i in range(10):
        tp = random.choice(['R', 'I'])
        op0 = random.randint(0, 31)
        op1 = random.randint(0, 31)
        op2 = random.randint(0, 31)
        imm12 = random.randint(-0x800, 0x7ff)
        imm20 = random.randint(0, 0xfffff)
        if tp == 'R':
            opc = random.choice(typeRop)
            op = '{opc} x{rd}, x{rs1}, x{rs2}'.format(
                opc=opc, rd=op0, rs1=op1, rs2=op2)
        elif tp == 'I':
            opc = random.choice(typeIop)
            op = '{opc} x{rd}, x{rs1}, {imm12}'.format(
                    opc=opc, rd=op0, rs1=op1, imm12=imm12)
        ops.append(op)
    ops_str = '\n'.join([f'  {op};' for op in ops])
    return ops_str


def main():
    code = code_template.format(random_op())
    with open('randomop.s', 'w') as f:
        f.write(code)


if __name__ == '__main__':
    main()
