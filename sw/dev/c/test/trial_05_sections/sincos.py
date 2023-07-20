import math

costable_str = '''
const int16_t cosTable[256] = {
%s
};
'''

vlist = []
for i in range(256):
    v = math.cos(float(i) / 256.0 * 2.0*math.pi)
    v16 = int(v * 256.0)
    vlist.append(v16)

lines = []
for i in range(16):
    lines.append(','.join([str(vlist[j+i*16]) for j in range(16)]))
s = ',\n'.join(lines)

# s = costable_str % ',\n'.join([str(v) for v in vlist])
s = costable_str % s

with open('sincos.c', 'w') as f:
    f.write(s)
