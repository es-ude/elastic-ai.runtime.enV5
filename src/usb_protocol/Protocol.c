#define SOURCE_FILE "USB-PROTOCOL"

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include "CException.h"

#include "DefaultCommands.h"
#include "include/UsbProtocol.h"

static usbProtocolReadCommand readHandle = NULL;
static usbProtocolSendData sendHandle = NULL;
static volatile usbProtocolCommandHandle commands[UINT8_C(0xFF)] = {0};

static void addCommand(uint8_t command, usbProtocolCommandHandle handle) {
    commands[command] = handle;
}
static uint8_t calculateChecksum(int numberOfArguments, va_list data) {
    uint8_t checksum = 0x00;
    for (size_t input = 0; input < numberOfArguments; input += 2) {
        uint8_t *byteArray = va_arg(data, uint8_t *);
        size_t byteArrayLength = va_arg(data, size_t);

        for (size_t index = 0; index < byteArrayLength; index++) {
            checksum ^= byteArray[index];
        }
    }

    return checksum;
}
static bool checksumPassed(uint8_t expectedChecksum, int numberOfArguments, ...) {
    va_list data;
    va_start(data, numberOfArguments);
    uint8_t actualChecksum = calculateChecksum(numberOfArguments, data);
    va_end(data);

    return (actualChecksum != expectedChecksum);
}

static void addDefaultFunctions(void) {
    addCommand(2, sendDeviceId);
    addCommand(3, sendDataToRam);
    addCommand(4, readDataFromRam);
    addCommand(5, sendDataToFlash);
    addCommand(6, readDataFromFlash);
    addCommand(7, runInferenceWithRamData);
    addCommand(8, runInferenceWithFlashData);
    addCommand(9, runTrainingWithRamData);
    addCommand(10, runTrainingWithFlashData);
}
void usbProtocolInit(usbProtocolReadCommand readFunction, usbProtocolSendData sendFunction) {
    if (readFunction == NULL || sendFunction == NULL) {
        Throw(USB_PROTOCOL_ERROR_NULL_POINTER);
    }

    readHandle = readFunction;
    sendHandle = sendFunction;

    addDefaultFunctions();
}

void usbProtocolAddCommand(size_t identifier, usbProtocolCommandHandle command) {
    if (command == NULL) {
        Throw(USB_PROTOCOL_ERROR_NULL_POINTER);
    }
    if (identifier > UINT8_MAX || identifier < ((UINT8_MAX / 2) + 1)) {
        Throw(USB_PROTOCOL_ERROR_INVALID_ID);
    }

    addCommand(identifier, command);
}

static void readBytes(uint8_t *data, size_t numberOfBytes) {
    for (size_t index = 0; index < numberOfBytes; index++) {
        while (readHandle(&data[index]) != USB_PROTOCOL_OKAY) {}
    }
}
static uint8_t readChecksum(void) {
    uint8_t checksum = 0;
    while (readHandle(&checksum) != USB_PROTOCOL_OKAY) {}
    return checksum;
}
static void sendAck(void) {
    // TODO: implement
}
static void sendNack(void) {
    // TODO: implement

    Throw(USB_PROTOCOL_ERROR_CHECKSUM_FAILED);
}
usbProtocolReceivedCommand_t *usbProtocolWaitForCommand(void) {
    uint8_t rawCommand[5];
    readBytes(rawCommand, 5);

    usbProtocolReceivedCommand_t *received = calloc(1, sizeof(usbProtocolReceivedCommand_t));
    received->command = rawCommand[0];
    received->payloadLength =
        rawCommand[1] << 24 | rawCommand[2] << 16 | rawCommand[3] << 8 | rawCommand[4];

    if (received->payloadLength != 0) {
        received->payload = calloc(received->payloadLength, sizeof(uint8_t));
        readBytes(received->payload, received->payloadLength);

        uint8_t checksum = readChecksum();
        checksumPassed(checksum, 4, rawCommand, 5, received->payload, received->payloadLength)
            ? sendAck()
            : sendNack();
    } else {
        uint8_t checksum = readChecksum();
        checksumPassed(checksum, 2, rawCommand, 5) ? sendAck() : sendNack();
    }

    return received;
}
void usbProtocolHandleCommand(usbProtocolReceivedCommand_t *commandToHandle) {
    commands[commandToHandle->command](commandToHandle->payload, commandToHandle->payloadLength);
}
