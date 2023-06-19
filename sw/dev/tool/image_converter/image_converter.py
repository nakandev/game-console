#!/usr/bin/env python

'''
# 画像ファイルからゲーム用ソースコード/バイナリを生成する
'''

# import cv2
from PIL import Image
import argparse
import re
import os

c_header_str = '''
#ifndef %s
#define %s
extern uint8_t %s_pal[%d];
extern uint8_t %s_data[%d];
#endif
'''

c_code_str = '''
#include "%s"
extern "C" {
uint8_t %s_pal[%d] = {
%s
};
uint8_t %s_data[%d] = {
%s
};
};
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

    image = image.convert('PA')
    print(image.getcolors())
    print(image.getpalette())

    name = os.path.basename(args.input)
    name = os.path.splitext(name)[0]
    name = 'img_' + re.sub(r'[ -/]', '_', name)
    print(name)

    w, h = image.size
    data = list()
    px = image.load()
    for y in range(0, 4):
        for x in range(0, 4):
            p = px[y, x]
            p = [p] if type(p) == int else p
            data.append(list(p))
            print(list(p[:]), end=' ')
        print('')


if __name__ == '__main__':
    main()
