from processordesign import regs, RegisterGroup, Register

regs += Register('pc')
regs += RegisterGroup('gpr')
regs.gpr = (
    Register('x0', 'zero'),
    Register('x1', 'ra'),
    Register('x2', 'sp'),
    Register('x3', 'gp'),
    Register('x4', ''),
    Register('x5', ''),
    Register('x6', ''),
    Register('x7', ''),
    Register('x8', ''),
    Register('x9', ''),
    Register('x10', ''),
    Register('x11', ''),
    Register('x12', ''),
    Register('x13', ''),
    Register('x14', ''),
    Register('x15', ''),
    Register('x16', ''),
    Register('x17', ''),
    Register('x18', ''),
    Register('x19', ''),
    Register('x20', ''),
    Register('x21', ''),
    Register('x22', ''),
    Register('x23', ''),
    Register('x24', ''),
    Register('x25', ''),
    Register('x26', ''),
    Register('x27', ''),
    Register('x28', ''),
    Register('x29', ''),
    Register('x30', ''),
    Register('x31', ''),
)

regs += RegisterGroup('csr')
regs.csr = {
    0: Register(),
}
