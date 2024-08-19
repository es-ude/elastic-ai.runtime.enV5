#define SOURCE_FILE "USB-PROTOCOL_DEFAULT_COMMANDS"

#include "string.h"

#include "CException.h"

#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Flash.h"
#include "Gpio.h"
#include "UsbProtocolCustomCommands.h"
#include "internal/DefaultCommands.h"
#include "internal/Tools.h"
#include "middleware.h"
#include "stub.h"

//! number of bytes always present in response (command + payload length + checksum)
#define BASE_LENGTH (sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t))

static uint32_t convertBytes(const uint8_t data[4]) {
    return data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
}

void readSkeletonId(__attribute__((unused)) const uint8_t *data,
                    __attribute((unused)) size_t length) {
    uint8_t skeletonId[16];
    modelGetId(skeletonId);

    usbProtocolMessage_t *msg = usbProtocolCreateMessage(2, skeletonId, 16);
    sendHandle(msg->message, msg->length);
    usbProtocolFreeMessageBuffer(msg);
    if (!usbProtocolWaitForAcknowledgement()) {
        Throw(USB_PROTOCOL_ERROR_HANDLE_EXECUTION_FAILED);
    }
}

void getChunkSize(__attribute__((unused)) const uint8_t *data,
                  __attribute((unused)) size_t length) {
    uint32_t payload = __builtin_bswap32(FLASH_BYTES_PER_PAGE);

    usbProtocolMessage_t *msg = usbProtocolCreateMessage(3, (uint8_t *)&payload, sizeof(payload));
    sendHandle(msg->message, msg->length);
    usbProtocolFreeMessageBuffer(msg);
    if (usbProtocolWaitForAcknowledgement()) {
        Throw(USB_PROTOCOL_ERROR_HANDLE_EXECUTION_FAILED);
    }
}

static void receiveData(uint32_t startSector, uint32_t length) {
    // TODO: implement
}
void sendDataToFlash(const uint8_t *data, size_t length) {
    uint32_t sectorAddress = convertBytes(&data[0]);
    uint32_t bytesToReceive = convertBytes(&data[4]);

    receiveData(sectorAddress, bytesToReceive);
}

static void sendData(uint32_t startAddress, uint32_t length) {
    // TODO: implement
}
void readDataFromFlash(const uint8_t *data, size_t length) {
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
    while (bytesToSend >= FLASH_BYTES_PER_PAGE) {
        usbProtocolMessage_t *msg = usbProtocolCreateMessage(
            9, buffer + (chunkId * FLASH_BYTES_PER_PAGE), FLASH_BYTES_PER_PAGE);
        sendHandle(msg->message, msg->length);
        usbProtocolFreeMessageBuffer(msg);
        if (!usbProtocolWaitForAcknowledgement()) {
            Throw(USB_PROTOCOL_ERROR_HANDLE_EXECUTION_FAILED);
        }

        bytesToSend -= FLASH_BYTES_PER_PAGE;
        chunkId++;
    }
    if (bytesToSend > 0) {
        usbProtocolMessage_t *msg =
            usbProtocolCreateMessage(9, buffer + (chunkId * FLASH_BYTES_PER_PAGE), bytesToSend);
        sendHandle(msg->message, msg->length);
        usbProtocolFreeMessageBuffer(msg);
    }
}
static void receiveInput(uint8_t *buffer, size_t bufferLength) {
    size_t bytesToRead = bufferLength;
    size_t chunkId = 0;
    uint8_t command;
    while (bytesToRead >= FLASH_BYTES_PER_PAGE) {
        readBytes(&command, 1);
        readBytes(buffer + (chunkId * FLASH_BYTES_PER_PAGE), FLASH_BYTES_PER_PAGE);
        bytesToRead -= FLASH_BYTES_PER_PAGE;
        chunkId++;
    }
    if (bytesToRead > 0) {
        readBytes(&command, 1);
        readBytes(buffer + (chunkId * FLASH_BYTES_PER_PAGE), bytesToRead);
    }
}
void runInference(const uint8_t *data, size_t length) {
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
