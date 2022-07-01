import serial
import sys
from Configuration import Configuration
ser = serial.Serial("/dev/tty.usbmodem14201", 500000)

bitfile = None

def verifyCorrectValueReceived(value, received, name):
    try:
        if value != received:
            print(name, 'incorrect! aborting.')
            sys.exit(0)
    except:
        print('did not receive', name, ':', received, '(', value, ')')
        sys.exit(0)

def writeValue(value, name):
   # data = bytearray([ (value << 24) & 0xff,(value << 16) & 0xff,(value << 8) & 0xff, value & 0xff  ])
    data= int(value).to_bytes( 4 ,"little", signed=False)
    ser.write(data)

 #   ser.write(value)

    print(name, 'of bitfile: ')
    receivedRaw= ser.readline()
    receivedRaw.strip()
    received = int(receivedRaw)
    print(received)
    print("received")
    verifyCorrectValueReceived(value, received, name)

def sendConfig(config):
    config.loadFile()
    ser.write(b'F')
    waitForAck()
    bitfile = open(config.filename, "rb")
    # skip the first bunch of data
    print('skipping', config.skip)
    bitfile.read(config.skip)

    address = config.address
    print('sending address', config.address)
    writeValue(config.address, "address")
    writeValue(config.size, "size")
    print(ser.readline())
    for i in range(9):
        print(ser.readline())
        print(ser.readline())
        print(ser.readline())

    # print('[chao_debug] wainting for flash erase finished\r\n')
    # give device time for some debug
          #  self.waitSerialReady(quiet=False)


    waitForAck()
    print('sending data')


def waitForAck():
    acknowledged=False
    while not acknowledged:
        data= ser.readline()
        str= data.decode("utf-8").strip()
        if(str=="ack"):
            acknowledged=True
            print("acked")

if __name__ == '__main__':
        config = Configuration("s15_p2.bit", 0x0,0x0)
        sendConfig(config)


#
# def sendConfig(self, config):
#
#
#
#     config.loadFile()
#
#     bitfile = None
#
#     ret = True
#     try:
#         print('waiting for ack...')
#         self.writeCommand(b'F')
#         ser.read(3)
#         # print('[chao_debug] command F sent.')
#
#         # sending bitfile
#         bitfile = open(config.filename, "rb")
#         # skip the first bunch of data
#         print('skipping', config.skip)
#         # skip the first bunch of data
#         bitfile.read(config.skip)
#
#         address = config.address
#         print('sending address', config.address)
#         self.writeValue(config.address, "address")
#
#         # sending size
#         # print('[chao_debug] config_size is ', config.size)
#         self.writeValue(config.size, "size")
#
#         # print('[chao_debug] wainting for flash erase finished\r\n')
#         # give device time for some debug
#         self.waitSerialReady(quiet=False)
#         print('sending data')
#
#         oldperc = -1
#
#         # self.ser.timeout = None
#         blockSize = 256
#         currentAddress = 0
#
#         while currentAddress < config.size:
#             # print('[chao_debug] in sending loop..\r\n')
#             perc = int(float(currentAddress) / config.size * 100)
#
#             if oldperc != perc:
#                 sys.stdout.write('\r{}%'.format(perc))
#                 sys.stdout.flush()
#                 # print('\r{}%'.format(perc))
#                 oldperc = perc
#
#             if (config.size - currentAddress) < blockSize:
#                 print("Last block!")
#                 blockSize = config.size - currentAddress
#                 # print(blockSize)
#
#             elif (config.size - currentAddress) == blockSize:
#                 print("Last full block!")
#
#             sending = bytearray(bitfile.read(blockSize))
#             # print('[chao_debug] a block is read out from bit file, type is',type(sending),',its size is:', len(sending))
#             # print('[chao_debug] data content', sending)
#
#             bytes_has_written = self.ser.write(sending)
#             self.ser.flush()
#
#
#             if not self.confirmCommand(sending[-1]):
#                 print("Could not confirm data!")
#
#                 if bitfile is not None:
#                     bitfile.close()
#                 ret = False
#                 break
#             currentAddress += blockSize
#             # print('[chao_debug] data has been send to the mcu wait for ack..\r\n')
#             self.waitSerialDone()
#
#         # # confirm the last
#         print('ready to proceed with uart')
#         self.waitSerialDone()
#     except KeyboardInterrupt:
#         pass
#     finally:
#         if bitfile is not None:
#             bitfile.close()
#
#     return ret
#
# def findPort(self, portTemplate):
#
#     ports = glob.glob(portTemplate)
#     # ports = glob.glob("/dev/tty.usbserial-A51ZP5TJ")
#     if len(ports) == 0:
#         print("no serial ports...")
#         time.sleep(2)
#         ports = glob.glob(portTemplate)
#         if len(ports) == 0:
#             print("still no serial ports...")
#             sys.exit(0)
#         else:
#             self.port = ports[0]
#     elif len(ports) == 1:
#         self.port = ports[0]
#     else:
#         print("too many serial ports...")
#         print(ports)
#         sys.exit(0)
#
# def isPortAvail(self):
#     print('Opening serial...', self.port, '@', self.baud)
#     exists = os.path.exists(self.port)
#     print("port", exists)
#     return exists
#
# def __init__(self, port, baudrate):
#     # self.port = port
#     self.baudrate=baudrate
#     try:
#         self.findPort(port)
#         if self.isPortAvail:
#             retries = 0
#             while not self.isPortAvail() and retries < 10:
#                 retries += 1
#                 time.sleep(0.5)
#                 print("waiting for port...")
#
#         self.ser = serial.Serial(self.port, self.baud, )
#         print('Serial available')
#     except KeyboardInterrupt:
#         print('Interrupted initialisation')
#         sys.exit(0)
#
# def writeConfigAndVerify():
#     bitfileFlasher = BitfileFlasher("/dev/tty.usbmodem*")
#     bitfileConfigs = BitfileConfigs()
#     assert bitfileFlasher.sendConfig(bitfileConfigs.ConfigPart1)
#
#
#
# if __name__ == "__main__":
#     writeConfigAndVerify()
