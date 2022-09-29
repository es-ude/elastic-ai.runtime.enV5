import sys

import numpy as np
from warnings import warn

MINI_FILE = 0x1000


class Configuration:
    filename = None
    address = None
    size = None
    skip = None
    destination = None
    special = None

    def __init__(this, fn, add, dest=0, mini=False, special=False):
        this.filename = fn
        this.address = add
        this.destination = dest
        this.special = special
        this.mini = mini

    # fetch size and skip
    def loadFile(this):
        print("loading file from %s" % this.filename)
        bitfile = open(this.filename, 'rb')

        if not bitfile:
            raise Exception("Bitfile not found at %s" % this.filename)

        # check file size
        datasave = list()
        textsave = list()
        isBinFile = ".bin" in this.filename
        if isBinFile:
            print("bin file detected, not skipping")
        savedata = not isBinFile

        size = 0
        while True:
            byte = bitfile.read(1)
            if not byte:
                break
            else:
                if savedata:
                    datasave.append(ord(byte))
                    textsave.append(byte)
                    # check for sync
                    if len(datasave) > 16:
                        if (np.array(datasave[-16:]) == 255).all(): savedata = False

                size += 1

        # don't skip if binfile
        if isBinFile:
            this.size = size
            this.skip = 0

        # skip first 12
        data = np.array(datasave[12:])
        text = np.array(textsave[12:])

        if data[0] != 1: print('Bitfile error 1')
        # a 
        # print(text[1])
        if data[2] != 0:
            print('Bitfile error 2')
            sys.exit(0)
        lengtha = data[3]

        # b
        # print(text[4+lengtha])
        if data[5 + lengtha] != 0:
            print('Bitfile error 3')
            sys.exit(0)
        lengthb = data[6 + lengtha]

        # c
        # print(text[7+lengtha+lengthb])
        if data[8 + lengtha + lengthb] != 0:
            print('Bitfile error 4')
            sys.exit(0)
        lengthc = data[9 + lengtha + lengthb]

        # d
        # print(text[10+lengtha+lengthb+lengthc])
        if data[11 + lengtha + lengthb + lengthc] != 0:
            print('Bitfile error 5')
            sys.exit(0)
        lengthd = data[12 + lengtha + lengthb + lengthc]

        # e
        lengthe = 0
        for i in range(4):
            lengthe += 256 ** (3 - i) * data[14 + lengtha + lengthb + lengthc + lengthd + i]
        skip = (12 + 14 + lengtha + lengthb + lengthc + lengthd + 4)

        bitfile.close()

        # use shorter file length for testing
        if this.special:
            print("Special file")
            this.size = size - skip
            this.skip = skip
        elif this.mini:
            print("Mini test")
            this.size = MINI_FILE
            this.skip = skip
        else:
            print('got size for', this.filename, lengthe, skip, size)
            this.size = lengthe
            this.skip = skip
