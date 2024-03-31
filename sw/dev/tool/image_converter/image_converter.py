#!/usr/bin/env python

'''
# 画像ファイルからゲーム用ソースコード/バイナリを生成する
'''

# import cv2
from PIL import Image
import argparse
import re
import os
import sys

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

coe_file_str = '''memory_initialization_radix=16;
memory_initialization_vector=
{data};
'''


def main():
    parser = argparse.ArgumentParser(
        prog='image_converter',
        description='convert image to source code'
    )
    parser.add_argument('input')
    parser.add_argument('-o', '--output')
    parser.add_argument('-t', '--type', choices=['bin', 'src', 'obj', 'coe', 'all'], default='bin')
    args = parser.parse_args()

    name = os.path.basename(args.input)
    name = re.sub(r'[ -/]', '_', name)
    print('name:', name)

    image = Image.open(args.input)
    assert image is not None

    print('image.mode:', image.mode)
    image = image.convert('P')

    pals = list()
    data = list()
    print('image.mode:', image.mode)
    if image.mode == 'P':
        print('image.palette.mode:', image.palette.mode)
        imgpals = image.getpalette()
        len_colors = len(image.getcolors())
        for i in range(len_colors):
            if image.palette.mode == 'RGB':
                col = imgpals[i * 3: i * 3 + 3]
                if 'transparency' in image.info and i == image.info['transparency']:
                    col = col + [0]
                else:
                    col = col + [255]
                # col32 = ABGR
                col32 = (col[3] << 24) | (col[2] << 16) | (col[1] << 8) | (col[0] << 0)
            elif image.palette.mode == 'RGBA':
                col = imgpals[i * 4: i * 4 + 4]
                col32 = (col[3] << 24) | (col[2] << 16) | (col[1] << 8) | (col[0] << 0)
            elif image.palette.mode == 'BGR;24':
                col = imgpals[i * 3: i * 3 + 3]
                if 'transparency' in image.info and i == image.info['transparency']:
                    col = col + [0]
                else:
                    col = col + [255]
                col32 = (col[3] << 24) | (col[0] << 16) | (col[1] << 8) | (col[2] << 0)
            else:
                print(f'Error: Not supported image palette mode: {image.palette.mode}')
                sys.exit(1)
            pals.append(col32)

        w, h = image.size
        for ty in range(h // 8):
            for tx in range(w // 8):
                for y in range(0, 8):
                    for x in range(0, 8):
                        p = image.getpixel((x + tx * 8, y + ty * 8))
                        data.append(p)
    else:
        print(f'Error: Not supported image mode: {image.mode}')
        sys.exit(1)

    out_type = args.type
    print('output.type:', out_type)
    if out_type == 'bin' or out_type == 'all':
        with open(name + '.pal', 'wb') as f:
            for col in pals:
                f.write(col.to_bytes(4, byteorder='little'))
        with open(name + '.tile', 'wb') as f:
            for d in data:
                f.write(d.to_bytes(1, byteorder='little'))
    if out_type == 'src' or out_type == 'all':
        h_str = c_header_str.format(
            name=name, palsize=len(pals), datasize=len(data)
        )

        n = 8  # newline num
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
    if out_type == 'coe' or out_type == 'all':
        n = 8  # newline num
        coe_pal_str = coe_file_str.format(
            data=',\n'.join([','.join([
                hex(e)[2:] for e in pals[i: i + n]]
            ) for i in range(0, len(pals), n)])
        )

        n = 64  # newline num
        coe_data_str = coe_file_str.format(
            data=',\n'.join([','.join([
                hex(e)[2:] for e in data[i: i + n]]
            ) for i in range(0, len(data), n)])
        )
        with open(name + '.pal.coe', 'w') as f:
            f.write(coe_pal_str)
        with open(name + '.tile.coe', 'w') as f:
            f.write(coe_data_str)


if __name__ == '__main__':
    main()
