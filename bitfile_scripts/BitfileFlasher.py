import time
import logging
import serial
import sys
import argparse
from Configuration import Configuration

bitfile = None
logging.basicConfig(level=logging.DEBUG)


def init_configuration(ser, config):
    config.loadfile()
    ser.write(b"P")
    time.sleep(0.5)
    ser.write(b"F")
    wait_for_ack(ser)


def erase_sectors(ser):
    num_erase_sectors = ser.readline()
    logging.info("Number of sectors to erase {}".format(num_erase_sectors.strip()))
    logging.info(ser.readline())
    wait_for_ack(ser)


def send_config(ser, config):
    init_configuration(ser, config)
    bitfile = open(config.filename, "rb")
    # skip the first bunch of data
    logging.info("Skipping bitfile header: {}".format(config.skip))
    bitfile.read(config.skip)

    logging.info("sending address {}".format(config.address))
    write_value(ser, config.address, "address")
    write_value(ser, config.size, "size")
    erase_sectors(ser)
    logging.info("sending data")
    send_data(ser, config, bitfile)
    bitfile.close()


def wait_for_ack(ser):
    acknowledged = False
    while not acknowledged:
        data = ser.readline()
        str = data.decode("utf-8").strip()
        if str == "ack":
            acknowledged = True


def write_value(ser, value, name):
    data = int(value).to_bytes(4, "little", signed=False)
    ser.write(data)
    received = int(ser.readline().strip())
    logging.info("{} of bitfile with value: {}".format(name, received))
    verify_correct_value_received(value, received, name)


def verify_correct_value_received(expected, received, name):
    try:
        if expected != received:
            logging.error("{} incorrect! Aborting.".format(name))
            sys.exit(0)
    except:
        logging.error(
            "did not receive {}. Received: {}  Expected: {}".format(
                name, received, expected
            )
        )
        sys.exit(0)


def send_data(ser, config, bitfile):
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
        oldPercentage = calculate_and_print_flash_progress(
            oldPercentage, (config.size - config_remaining), config.size
        )
        sending = bytearray(bitfile.read(blockSize))
        bytes_has_written = ser.write(sending)
        if lastBlock == True:
            logging.info("Number of bytes in last block: {}".format(bytes_has_written))
            logging.debug(ser.readline().decode("utf-8"))

        wait_for_ack(ser)
        currentAddress += blockSize
        config_remaining -= blockSize

    wait_for_ack(ser)
    logging.info("Flashing succeeded.")


def calculate_and_print_flash_progress(oldPercentage, sent_config, configSize):
    percentage = int(float(sent_config) / configSize * 100)
    if oldPercentage != percentage:
        sys.stdout.write("\r{}%".format(percentage))
        sys.stdout.flush()
    return percentage


def verify_bitfile(ser, config):
    # ser.write(b'P')
    time.sleep(0.5)
    config.loadfile()
    bitfile = open(config.filename, "rb")
    # skip the first bunch of data
    logging.info("Skipping bitfile header: {}".format(config.skip))
    bitfile.read(config.skip)
    ser.write(b"V")
    wait_for_ack(ser)
    write_value(ser, config.address, "address")
    write_value(ser, config.size, "size")

    block_size = 256
    numBlock = 0
    position = config.address
    config_remaining = config.size

    while config_remaining > 0:
        if config_remaining < block_size:
            block_size = config_remaining
            logging.debug(ser.readline().decode("utf-8"))

        flash_data_block = receive_and_prepare_flash_data(ser)
        expected_block = [int(i) for i in bytearray(bitfile.read(block_size))]
        if len(flash_data_block) != len(expected_block):
            logging.error(
                "Different length of blocks at block number {} \t Expected: {}, \t On Device {}".format(
                    numBlock, len(expected_block), len(flash_data_block)
                )
            )
            sys.exit(0)
        else:
            for i in range(0, block_size):
                if expected_block[i] != flash_data_block[i]:
                    logging.info(
                        "Expected block: {:02X}  -  Received block: {:02X}".format(
                            expected_block[i], flash_data_block[i]
                        )
                    )
                    logging.error(
                        "Bitfile Verification failed at byte {} of block {}".format(
                            i, numBlock
                        )
                    )
                    sys.exit(0)
        # assert (expected_block[i] == flash_data_block[i]), "Bitfile Verification failed at block {}".format(numBlock)

        config_remaining -= block_size
        position += block_size
        numBlock += 1
        wait_for_ack(ser)

    wait_for_ack(ser)
    bitfile.close()
    logging.info("Bitfile has been verified. You can proceed with your application.")


def receive_and_prepare_flash_data(ser):
    data = ser.readline().strip()
    if "Debug" in data.decode("utf-8"):
        logging.debug(data)
        flash_data_block = ser.readline().strip().split(bytearray("###", "utf-8"))
    else:
        flash_data_block = data.split(bytearray("###", "utf-8"))
    flash_data_block.pop()
    return [int(i) for i in flash_data_block]


def build_parser():
    parser = argparse.ArgumentParser(description="Flash Bitfile")
    parser.add_argument(
        "--port", "-p", dest="port", required=True, help="Elastic Node v5 Serial Port"
    )
    parser.add_argument(
        "--baudrate", "-b", dest="baudrate", type=int, default=5000000, help="Baudrate"
    )
    parser.add_argument("source_file", help="Path of Bitfile")
    parser.add_argument("destination_address", help="location in flash")
    return parser


if __name__ == "__main__":
    parser = build_parser()
    arguments = parser.parse_args()
    ser = serial.Serial(arguments.port, arguments.baudrate)
    config = Configuration(
        "bitfile_scripts/bitfiles/" + arguments.source_file,
        int(arguments.destination_address, 16),
    )
    # sendConfig(ser, config)
    verify_bitfile(ser, config)
    ser.close()
