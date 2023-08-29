import sys
import numpy as np


class Configuration:
    filename = None
    address = None
    size = None
    skip = None
    header_info = None

    def __init__(self, fn, add):
        self.filename = fn
        self.address = add

    def loadfile(self):
        print("loading file from %s" % self.filename)
        with open(self.filename, "rb") as configfile:
            # check file size
            isBinFile = ".bin" in self.filename
            if isBinFile:
                self.load_binfile(configfile)
                return
            else:
                self.load_bitfile(configfile)

    def load_binfile(self, binfile):
        print("bin file detected, not skipping")
        size = 0
        while binfile.read(1):
            size += 1
        self.size = size
        self.skip = 0
        print(size)

    def load_bitfile(self, bitfile):
        isHeaderData = 1
        size = 0
        datasave = list()
        textsave = list()

        while byte := bitfile.read(1):
            if isHeaderData:
                self.store_headerdata(byte, datasave, textsave)
                iƒsHeaderData = self.is_headerdata(datasave)
            size += 1

        lengths = self.parse_header(datasave, textsave)
        print("got size for", self.filename, lengths[0], lengths[1], size)
        self.size = lengths[0]
        self.skip = lengths[1]

    #   def load_bitfile(self, bitfile):
    def store_headerdata(self, data, datasave, textsave):
        datasave.append(ord(data))
        textsave.append(data)

    # data for the FPGA starts with a lot of 0xFF bytes. Before that there's a header for vivado
    def is_headerdata(self, datasave):
        if len(datasave) > 16:
            if (np.array(datasave[-16:]) == 255).all():
                return False
        return True

    # the header of the .bit file is for vivado. we have to skip the header when sending the bitfile to the mcu.
    # header is parseable. Special bit frequencies at point a,b,c,d,e, followed by the length to the next point.
    # at length e there's the length of the configuration file without header
    def parse_header(self, header_data, header_info):
        # skip first 12 not important bytes
        design_descr_start = 13
        if chr(header_data[design_descr_start]) != "a":
            print(
                "Bitfile header parsing error 1: design description starting bit sequence wrong"
            )
            sys.exit(0)

        design_description_length = +header_data[design_descr_start + 2]
        part_description_start = design_description_length + design_descr_start + 3

        if chr(header_data[part_description_start]) != "b":
            print(
                "Bitfile header parsing error 2: part_description starting sequence wrong"
            )
            sys.exit(0)

        part_description_length = header_data[part_description_start + 2]
        date_start = part_description_length + part_description_start + 3

        if chr(header_data[date_start]) != "c":
            print("Bitfile header parsing error 3: date starting sequence wrong")
            sys.exit(0)

        date_length = header_data[date_start + 2]
        time_start = date_start + date_length + 3

        if chr(header_data[time_start]) != "d":
            print("Bitfile header parsing error 4: time starting sequence wrong")
            sys.exit(0)

        time_length = header_data[time_start + 2]
        config_start = time_start + time_length + 3

        config_length = 0

        for i in range(4):
            config_length += 256 ** (3 - i) * header_data[config_start + 1 + i]

        skip = config_start + 5
        return config_length, skip
