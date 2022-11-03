import time
import logging
import serial
import sys
import argparse
from Configuration import Configuration

bitfile = None
logging.basicConfig(level=logging.DEBUG)


def initConfiguration(ser, config):
    config.loadfile()
    ser.write(b'P')
    time.sleep(0.5)
    ser.write(b'F')
    waitForAck(ser)


def sendConfig(ser, config):
    initConfiguration(ser, config)
    bitfile = open(config.filename, "rb")
    # skip the first bunch of data
    logging.info("Skipping bitfile header: {}".format(config.skip))
    bitfile.read(config.skip)

    logging.info("sending address {}".format(config.address))
    writeValue(ser, config.address, "address")
    writeValue(ser, config.size, "size")
    numEraseSectors = ser.readline()
    logging.info("Number of sectors to erase {}".format(numEraseSectors.strip()))
    logging.info(ser.readline())
    waitForAck(ser)
    logging.info("sending data")
    sendData(ser, config, bitfile)
    bitfile.close()


def waitForAck(ser):
    acknowledged = False
    while not acknowledged:
        data = ser.readline()
        str = data.decode("utf-8").strip()
        if (str == "ack"):
            acknowledged = True


def writeValue(ser, value, name):
    data = int(value).to_bytes(4, "little", signed=False)
    ser.write(data)
    received = int(ser.readline().strip())
    logging.info("{} of bitfile with value: {}".format(name, received))
    verifyCorrectValueReceived(value, received, name)


def verifyCorrectValueReceived(expected, received, name):
    try:
        if expected != received:
            logging.error("{} incorrect! Aborting.".format(name))
            sys.exit(0)
    except:
        logging.error("did not receive {}. Received: {}  Expected: {}".format(name, received, expected))
        sys.exit(0)


def sendData(ser, config, bitfile):
    oldPercentage = -1

    blockSize = 256
    currentAddress = config.address
    config_remaining = config.size
    lastBlock = False
    while config_remaining > 0:
        if config_remaining <= blockSize:
            blockSize = config_remaining
            logging.info("Sending Last block!")
            lastBlock = True
        oldPercentage = calculateAndPrintFlashProgress(oldPercentage, (config.size - config_remaining), config.size)
        sending = bytearray(bitfile.read(blockSize))
        bytes_has_written = ser.write(sending)
        if lastBlock == True:
            logging.info("Number of bytes in last block: {}".format(bytes_has_written))
            logging.debug(ser.readline())

        waitForAck(ser)
        currentAddress += blockSize
        config_remaining -= blockSize

    waitForAck(ser)
    logging.info("Flashing succeeded.")


def calculateAndPrintFlashProgress(oldPercentage, sent_config, configSize):
    percentage = int(float(sent_config) / configSize * 100)
    if oldPercentage != percentage:
        sys.stdout.write('\r{}%'.format(percentage))
        sys.stdout.flush()
    return percentage


def verifyBitfile(ser, config):
    config.loadfile()
    bitfile = open(config.filename, "rb")
    # skip the first bunch of data
    logging.info("Skipping bitfile header: {}".format(config.skip))
    bitfile.read(config.skip)
    ser.write(b'V')
    waitForAck(ser)
    writeValue(ser, config.address, "address")
    writeValue(ser, config.size, "size")

    blockSize = 256
    numBlock = 0
    position = config.address
    config_remaining = config.size

    while config_remaining > 0:
        if config_remaining < blockSize:
            blockSize = config_remaining

        flash_data_block = receive_and_prepare_flash_data()
        expected_block = [int(i) for i in bytearray(bitfile.read(blockSize))]
        if len(flash_data_block) != len(expected_block):
            logging.error("Different length of blocks at block number {} \t Expected: {}, \t On Device {}"
                          .format(numBlock,len(expected_block),len(flash_data_block)))
            sys.exit(0)
        else:
            for i in range(blockSize):
                assert (expected_block[i] == flash_data_block[i]), "Bitfile Verification failed"

        config_remaining -= blockSize
        position += blockSize
        numBlock += 1
        waitForAck(ser)

    waitForAck(ser)
    bitfile.close()


def receive_and_prepare_flash_data():
    logging.debug(ser.readline())
    flash_data_block = ser.readline().strip().split(bytearray("###", 'utf-8'))
    flash_data_block.pop()
    return [int(i) for i in flash_data_block]


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Flash Bitfile')
    parser.add_argument("--port", "-p", dest="port", required=True, help="Elastic Node v5 Serial Port")
    parser.add_argument("--baudrate", "-b", dest="baudrate", type=int, default=5000000, help="Baudrate")
    parser.add_argument("source_file", help="Path of Bitfile")
    parser.add_argument("destination_address", help="location in flash")
    arguments = parser.parse_args()
    ser = serial.Serial(arguments.port, arguments.baudrate)
    config = Configuration("bitfile_scripts/bitfiles/" + arguments.source_file, int(arguments.destination_address, 16))
    sendConfig(ser, config)
    verifyBitfile(ser, config)
    ser.close()
