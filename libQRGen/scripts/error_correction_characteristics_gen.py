#!/usr/bin/env python3

# Copyright 2024 Benjamin Lutz.
#
# This file is part of QRGen. QRGen is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.


import re
import sys


def main():
    '''This script takes the numbers in error_correction_characteristics_raw.txt,
    which where copied and pasted from the ISO/IEC 18004:2015 PDF, and converts
    the relevant numbers into C code.
    '''
    pat = re.compile(r'\((\d+),(\d+),(\d+)\)')
    version = 0
    with open('error_correction_characteristics_raw.txt') as f:
        lines = tuple(map(lambda s: re.sub('\\s', '', s), f.readlines()))

    i = 0
    while i < len(lines):
        if version >= 40: break
        
        version = int(lines[i])
        i += 1
        totalCount = int(lines[i])
        i += 1
        
        sys.stdout.write('{{')
        
        while True:
            sys.stdout.write('{{')
            ecLevel = lines[i]
            i += 1
            assert(ecLevel in ('L', 'M', 'Q', 'H'))
            ecwCount = int(lines[i])
            i += 1
            for ecbGroup in (0, 1):
                sys.stdout.write('{{')
                ecbCount = lines[i]
                i += 1
                match = re.match(pat, lines[i])
                i += 1
                if match is not None:
                    sys.stdout.write(', '.join((ecbCount, match.group(1), match.group(2))))
                if ecbGroup == 0:
                    if i + 1 >= len(lines) or re.match(pat, lines[i + 1]) is None:
                        sys.stdout.write('}}, {{0, 0, 0}}')
                        break
                    sys.stdout.write('}}, ')
                else:
                    sys.stdout.write('}}')
            if ecLevel == 'H':
                sys.stdout.write('}}')
            else:
                sys.stdout.write('}}, ')
            
            if ecLevel == 'H': break
                    
        sys.stdout.write('}}, // version %s\n' % (version,))

if __name__ == '__main__':
    main()
