#include "math.h"
#include "stdlib.h"
#include "string.h"

#include "CException.h"
#include "hardware/spi.h"

#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Flash.h"
#include "Gpio.h"
#include "Spi.h"
#include "include/UsbProtocolCustomCommands.h"
#include "internal/DefaultCommands.h"
#include "internal/Tools.h"
#include "middleware.h"
#include "stub.h"

//! number of bytes always present in response (command + payload length + checksum)
#define BASE_LENGTH (sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t))

#define MAX_RETRIES (5)

static spiConfiguration_t flashSpi = {
    .spiInstance = FLASH_SPI_MODULE,
    .baudrate = FLASH_SPI_BAUDRATE,
    .sckPin = FLASH_SPI_CLOCK,
    .misoPin = FLASH_SPI_MISO,
    .mosiPin = FLASH_SPI_MOSI,
    .csPin = FLASH_SPI_CS,
};

static flashConfiguration_t flash = {
    .flashSpiConfiguration = &flashSpi,
    .flashBytesPerPage = FLASH_BYTES_PER_PAGE,
    .flashBytesPerSector = FLASH_BYTES_PER_SECTOR,
};

void readSkeletonId(__attribute__((unused)) const uint8_t *data,
                    __attribute((unused)) size_t length) {
    uint8_t skeletonId[16];
    modelGetId(skeletonId);

    usbProtocolMessage_t message;
    message.command = 2;
    message.payloadLength = 16;
    message.payload = skeletonId;
    if (!usbProtocolSendMessage(&message)) {
        Throw(USB_PROTOCOL_ERROR_HANDLE_EXECUTION_FAILED);
    }
}

void getChunkSize(__attribute__((unused)) const uint8_t *data,
                  __attribute((unused)) size_t length) {
    uint32_t payload = __builtin_bswap32(FLASH_BYTES_PER_PAGE);
    usbProtocolMessage_t message;
    message.command = 3;
    message.payloadLength = sizeof(payload);
    message.payload = (uint8_t *)&payload;
    if (!usbProtocolSendMessage(&message)) {
        Throw(USB_PROTOCOL_ERROR_HANDLE_EXECUTION_FAILED);
    }
}

static void eraseFlash(uint32_t startSector, uint32_t length) {
    for (size_t index = 0; index < (size_t)ceilf((float)length / FLASH_BYTES_PER_SECTOR); index++) {
        flashEraseSector(&flash, startSector + (index * FLASH_BYTES_PER_SECTOR));
    }
}
static void receiveData(uint32_t startSector, uint32_t length) {
    size_t chunkId = 0;
    uint8_t nackCounter = 0;
    while (chunkId < (size_t)(ceilf((float)length / FLASH_BYTES_PER_PAGE))) {
        if (nackCounter >= MAX_RETRIES) {
            Throw(USB_PROTOCOL_ERROR_CHECKSUM_FAILED);
        }

        usbProtocolMessage_t message;
        if (!usbProtocolReadMessage(&message)) {
            nackCounter++;
            continue;
        }

        flashWritePage(&flash,
                       (startSector * FLASH_BYTES_PER_SECTOR) + (chunkId * FLASH_BYTES_PER_PAGE),
                       message.payload, message.payloadLength);
        free(message.payload);

        chunkId++;
        nackCounter = 0;
    }
}
void sendDataToFlash(const uint8_t *data, __attribute((unused)) size_t length) {
    uint32_t sectorAddress = convertBytes(&data[0]);
    uint32_t bytesToReceive = convertBytes(&data[4]);

    eraseFlash(sectorAddress, bytesToReceive);
    receiveData(sectorAddress, bytesToReceive);
}

static void sendData(uint32_t startAddress, uint32_t length) {
    size_t bytesToSend = length;
    size_t chunkId = 0;
    uint8_t nackCounter = 0;
    while (chunkId <= (size_t)(ceilf((float)length / FLASH_BYTES_PER_PAGE))) {
        usbProtocolMessage_t message;
        message.command = 5;
        if (bytesToSend > FLASH_BYTES_PER_PAGE) {
            message.payloadLength = FLASH_BYTES_PER_PAGE;
            uint8_t data[FLASH_BYTES_PER_PAGE];
            data_t buffer = {
                .length = FLASH_BYTES_PER_PAGE,
                .data = data,
            };
            flashReadData(&flash, startAddress + (chunkId * FLASH_BYTES_PER_PAGE), &buffer);
            message.payload = data;

            bytesToSend -= FLASH_BYTES_PER_PAGE;
        } else {
            message.payloadLength = bytesToSend;
            uint8_t data[bytesToSend];
            data_t buffer = {
                .length = bytesToSend,
                .data = data,
            };
            flashReadData(&flash, startAddress + (chunkId * FLASH_BYTES_PER_PAGE), &buffer);
            message.payload = data;
        }

        if (!usbProtocolSendMessage(&message)) {
            nackCounter++;
            continue;
        }

        chunkId++;
        nackCounter = 0;
    }
}
void readDataFromFlash(const uint8_t *data, __attribute((unused)) size_t length) {
    uint32_t startAddress = convertBytes(&data[0]);
    uint32_t bytesToReceive = convertBytes(&data[4]);

    sendData(startAddress, bytesToReceive);
}

void setFpgaPower(const uint8_t *data, __attribute((unused)) size_t length) {
    data[0] == 0xFF ? env5HwControllerFpgaPowersOn() : env5HwControllerFpgaPowersOff();
}

void setFpgaLeds(const uint8_t *data, __attribute((unused)) size_t length) {
    middlewareSetFpgaLeds(data[0]);
}

void setMcuLeds(const uint8_t *data, __attribute((unused)) size_t length) {
    gpioSetPin(LED0_GPIO, data[0] & 0x01 ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    gpioSetPin(LED1_GPIO, data[0] & 0x02 ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    gpioSetPin(LED2_GPIO, data[0] & 0x04 ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
}

static void sendOutput(uint8_t *buffer, size_t bufferLength) {
    size_t bytesToSend = bufferLength;
    size_t chunkId = 0;
    uint8_t nackCounter = 0;
    while (chunkId < (size_t)ceilf((float)bufferLength / FLASH_BYTES_PER_PAGE)) {
        if (nackCounter >= MAX_RETRIES) {
            Throw(USB_PROTOCOL_ERROR_HANDLE_EXECUTION_FAILED);
        }

        usbProtocolMessage_t message;
        message.command = 9;
        if (bytesToSend >= FLASH_BYTES_PER_PAGE) {
            bytesToSend -= FLASH_BYTES_PER_PAGE;

            message.payloadLength = FLASH_BYTES_PER_PAGE;
            message.payload = buffer + (chunkId * FLASH_BYTES_PER_PAGE);
        } else {
            message.payloadLength = bytesToSend;
            message.payload = buffer + (chunkId * FLASH_BYTES_PER_PAGE);
        }

        if (!usbProtocolSendMessage(&message)) {
            nackCounter++;
            continue;
        }
        chunkId++;
        nackCounter = 0;
    }
}
static void receiveInput(uint8_t *buffer, size_t bufferLength) {
    size_t chunkId = 0;
    uint8_t nackCounter = 0;
    while (chunkId < (size_t)ceilf((float)bufferLength / FLASH_BYTES_PER_PAGE)) {
        if (nackCounter >= MAX_RETRIES) {
            Throw(USB_PROTOCOL_ERROR_READ_FAILED);
        }

        usbProtocolMessage_t message;
        if (!usbProtocolReadMessage(&message)) {
            nackCounter++;
            continue;
        }
        memcpy(buffer + (chunkId * FLASH_BYTES_PER_PAGE), message.payload, message.payloadLength);
        free(message.payload);

        chunkId++;
        nackCounter = 0;
    }
}
void runInference(const uint8_t *data, __attribute((unused)) size_t length) {
    uint32_t inputLength = convertBytes(&data[0]);
    uint32_t outputLength = convertBytes(&data[4]);
    uint32_t acceleratorAddress = convertBytes(&data[8]);
    uint8_t acceleratorId[16];
    memcpy(acceleratorId, &data[12], 16);

    uint8_t networkInput[inputLength];
    receiveInput(networkInput, inputLength);

    if (modelDeploy(acceleratorAddress, acceleratorId)) {
        uint8_t networkOutput[outputLength];
        modelPredict(networkInput, inputLength, networkOutput, outputLength);
        sendOutput(networkOutput, outputLength);
    }
}
