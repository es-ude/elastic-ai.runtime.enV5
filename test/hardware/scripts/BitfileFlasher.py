import time

import serial
import sys
from Configuration import Configuration

ser = serial.Serial("/dev/tty.usbmodem14101", 5000000)


bitfile = None

def waitForAck():
    acknowledged=False
    while not acknowledged:
        data= ser.readline()
        str= data.decode("utf-8").strip()
        if(str=="ack"):
            acknowledged=True
        # print("acked")
def verifyCorrectValueReceived(value, received, name):
    try:
        if value != received:
            print(name, 'incorrect! aborting.')
            sys.exit(0)
    except:
        print('did not receive', name, ':', received, '(', value, ')')
        sys.exit(0)

def writeValue(value, name):

    data = int(value).to_bytes( 4 ,"little", signed=False)

    ser.write(data)


    print(name, 'of bitfile: ')

    receivedRaw= ser.readline()
    print(receivedRaw)
    receivedRaw.strip()
    received = int(receivedRaw)
    print(received)
    print("received")
    verifyCorrectValueReceived(value, received, name)


def testBitfileOut(config):
    config.loadFile()
    bitfile = open(config.filename, "rb")
    # skip the first bunch of data
    print('skipping', config.skip)
    bitfile.read(config.skip)
    expected_block =bytearray(bitfile.read(256))
    #print(expected_block[0])
    print(expected_block[:])
   # for i in range(256):
     #   print(expected_block[i])

def verifyFlashData():
    print("verify Flash")
    size=538880
    numBlock=0
    last_num_block=-1
    errorCounter=0
    ser.write(b'V')

    waitForAck()
    address=0x0
    writeValue(address, "address")
    writeValue(size, "size")

    data=[i for i in range(256)]

    for i in range(2105):
        flash_data_block=ser.readline().strip().split(bytearray("###", 'utf-8'))
        flash_data_block.pop()
        flash_data_block=[int(i) for i in flash_data_block]
        expected_block =data

        for i in range(256):
            if expected_block[i]!=flash_data_block[i]:
                errorCounter+=1
                if(last_num_block!=numBlock):
                    print(numBlock)
                last_num_block=numBlock

        numBlock+=1
        waitForAck()
    print(errorCounter)
    waitForAck()

def verifyBitfile(config):
    config.loadFile()
    bitfile = open(config.filename, "rb")
    # skip the first bunch of data
    print('skipping', config.skip)
    bitfile.read(config.skip)
    errorCounter = 0
    ser.write(b'V')

    waitForAck()
    writeValue(config.address, "address")
    writeValue(config.size, "size")

    blockSize = 256
    numBlock = 0
    last_num_block =- 1
    position = config.address
    remaining = config.size

    while remaining > 0:
        if remaining < blockSize:
            blockSize = remaining

        print(ser.readline())
        flash_data_block=ser.readline().strip().split(bytearray("###", 'utf-8'))
        flash_data_block.pop()
        flash_data_block=[int(i) for i in flash_data_block]
       # flash_data_block=ser.readline()
       # flash_data_block = bytearray(flash_data_block.strip())

        expected_block =[int(i) for i in bytearray(bitfile.read(blockSize))]
     #   print(expected_block[0])

        if numBlock < 12:
            length_flash=0
            length_expected=0
            for i in range (len(flash_data_block)):
                print(flash_data_block[i], end=" ")
                length_flash=length_flash+1
            print()
            for i in range (len(expected_block)):
                print(expected_block[i], end=" ")
                length_expected=length_expected+1
            print()
            print("length of flash: ", length_flash, " length expected: ", length_expected)

        if len(flash_data_block) != len(expected_block):
            print("Different length of blocks at block number {} \t Expected: {}, \t On Device {}".format(numBlock, expected_block, flash_data_block))
        else:
            for i in range(blockSize):

                if expected_block[i]!=flash_data_block[i]:
                    errorCounter+=1
                    if(last_num_block!=numBlock):
                        print(numBlock)
                    last_num_block=numBlock

        remaining-=blockSize
        position+=blockSize
        numBlock+=1
        waitForAck()


    print("Total errors: {}".format(errorCounter))

    waitForAck()


def sendConfig(config):

        config.loadFile()
        ser.write(b'P')
        time.sleep(0.5)
        ser.write(b'F')
        waitForAck()
        bitfile = open(config.filename, "rb")
        # skip the first bunch of data
        print('skipping', config.skip)
        bitfile.read(config.skip)

        address = config.address
        print('sending address', config.address)
        writeValue(config.address, "address")
       # writeValue(config.size, "size")
        writeValue(config.size, "size")
        numBlocks=ser.readline()
        print("number of sectors to erase:",numBlocks)


        for i in range(int(numBlocks.strip())):
            print(ser.readline())
            print(ser.readline())
            print(ser.readline())

        # print('[chao_debug] wainting for flash erase finished\r\n')
        # give device time for some debug
              #  self.waitSerialReady(quiet=False)


        waitForAck()
        print('sending data')
        sendData(config, bitfile)
      #  sendDummyData()
        bitfile.close()
        verifyBitfile(config)
        #verifyFlashData()

def sendDummyData():
    currentAddress=0
    size=538880
    data=[i for i in range(256)]
    sending = bytearray(data)
    print(sending)
    for i in range(2105):
        bytes_has_written = ser.write(sending)
        waitForAck()
    waitForAck()

def sendData(config, bitfile):
    oldperc = -1

    # self.ser.timeout = None
    blockSize = 256
    currentAddress = config.address
    numBlock=0
    lastBlock=False
    while currentAddress < config.size:
        # print('[chao_debug] in sending loop..\r\n')
        perc = int(float(currentAddress) / config.size * 100)

        if oldperc != perc:
            sys.stdout.write('\r{}%'.format(perc))
            sys.stdout.flush()
            # print('\r{}%'.format(perc))
            oldperc = perc

        if (config.size - currentAddress) < blockSize:
            print("Last block!")
            blockSize = config.size - currentAddress
            print(blockSize)
            # print(blockSize)
            lastBlock=True

        elif (config.size - currentAddress) == blockSize:
            print("Last full block!")
            lastBlock=True

        sending = bytearray(bitfile.read(blockSize))
        if lastBlock==True:
            print("inbefore write to device")
        # print('[chao_debug] a block is read out from bit file, type is',type(sending),',its size is:', len(sending))
        # print('[chao_debug] data content', sending)
        bytes_has_written = ser.write(sending)
        if lastBlock==True:
            print(bytes_has_written)
        ser.flush()
        #ser.flush()

        # if not confirmCommand(sending[-1]):
        #     print("Could not confirm data!")

            # if bitfile is not None:
            #     bitfile.close()
            # ret = False
            # break
        if numBlock < 10 or lastBlock==True:
            print(ser.readline())
        numBlock+=1
        waitForAck()
        currentAddress += blockSize
        # print('[chao_debug] data has been send to the mcu wait for ack..\r\n')

    # # confirm the last
    print('ready to proceed with uart')
    waitForAck()




if __name__ == '__main__':
       # config = Configuration("bitstream_led_blink/led_test.bit", 0x0000,0x0000)
        config = Configuration("led_run_10_times_faster/led_test.bit", 0x0000,0x0000)

        sendConfig(config)
      #  testBitfileOut(config)
