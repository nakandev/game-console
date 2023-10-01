#!/usr/bin/env python

'''
# 画像ファイルからゲーム用ソースコード/バイナリを生成する
'''

# import cv2
from PIL import Image
import argparse
import re
import os

c_header_str = '''#pragma once
#include <cstdint>
#ifndef arrayof
#define arrayof(x) (sizeof(x)/sizeof(x[0]))
#endif
extern "C" {{
extern uint32_t {name}_pal[{palsize}];
extern uint8_t {name}_data[{datasize}];
}};
'''

c_code_str = '''#include <cstdint>
extern "C" {{
uint32_t {name}_pal[{palsize}] = {{
{pal}
}};
uint8_t {name}_data[{datasize}] = {{
{data}
}};
}};
'''


def main():
    parser = argparse.ArgumentParser(
        prog='image_converter',
        description='convert image to source code'
    )
    parser.add_argument('input')
    parser.add_argument('-o', '--output')
    args = parser.parse_args()

    image = Image.open(args.input)
    assert image is not None

    # image = image.convert('PA')
    print('mode:', image.mode)
    # print('palette :', image.getpalette())
    # print('pixel(0, 0) :', image.getpixel((0, 0)))
    # print('colors :', image.getcolors())

    name = os.path.basename(args.input)
    # name = os.path.splitext(name)[0]
    name = 'img_' + re.sub(r'[ -/]', '_', name)
    print('name:', name)

    imgpals = image.getpalette()
    pals = list()
    for i in range(len(image.getcolors())):
        col = imgpals[i * 3: i * 3 + 3]
        if i == 0:
            col = col + [0]
        else:
            col = col + [255]
        col32 = (col[0] << 0) | (col[1] << 8) | (col[2] << 16) | (col[3] << 24)
        pals.append(col32)

    w, h = image.size
    data = list()
    for ty in range(h // 8):
        for tx in range(w // 8):
            for y in range(0, 8):
                for x in range(0, 8):
                    p = image.getpixel((x + tx * 8, y + ty * 8))
                    data.append(p)
                    # print(list(p[:]), end=' ')
                    # print(p, end=' ')
                # print('')

    h_str = c_header_str.format(
        name=name, palsize=len(pals), datasize=len(data)
    )

    n = 8
    dstr = ',\n'.join([','.join([
        str(e) for e in data[i: i + n]]
    ) for i in range(0, len(data), n)])
    c_str = c_code_str.format(
        name=name, palsize=len(pals), datasize=len(data),
        pal=','.join([hex(i) for i in pals]),
        data=dstr
    )
    with open(name + '.h', 'w') as f:
        f.write(h_str)
    with open(name + '.cpp', 'w') as f:
        f.write(c_str)
    with open(name + '.pal.bin', 'wb') as f:
        for col in pals:
            f.write(col.to_bytes(4, byteorder='little'))
    with open(name + '.tile.bin', 'wb') as f:
        for d in data:
            f.write(d.to_bytes(1, byteorder='little'))


if __name__ == '__main__':
    main()
