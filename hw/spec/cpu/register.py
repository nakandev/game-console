from processordesign import regs, RegisterGroup, Register

regs += Register('pc')
regs += RegisterGroup('gpr')
regs.gpr = (
    Register('x0', 'zero'),
    Register('x1', 'ra'),
    Register('x2', 'sp'),
    Register('x3', 'gp'),
    Register('x4', 'tp'),
    Register('x5', 't0'),
    Register('x6', 't1'),
    Register('x7', 't2'),
    Register('x8', 's0'),  # fp
    Register('x9', 's1'),
    Register('x10', 'a0'),
    Register('x11', 'a1'),
    Register('x12', 'a2'),
    Register('x13', 'a3'),
    Register('x14', 'a4'),
    Register('x15', 'a5'),
    Register('x16', 'a6'),
    Register('x17', 'a7'),
    Register('x18', 's2'),
    Register('x19', 's3'),
    Register('x20', 's4'),
    Register('x21', 's5'),
    Register('x22', 's6'),
    Register('x23', 's7'),
    Register('x24', 's8'),
    Register('x25', 's9'),
    Register('x26', 's10'),
    Register('x27', 's11'),
    Register('x28', 't3'),
    Register('x29', 't4'),
    Register('x30', 't5'),
    Register('x31', 't6'),
)

regs += RegisterGroup('csr')
regs.csr = {
    0: Register(),
}
