#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "CException.h"

#include "UsbProtocolBase.h"
#include "UsbProtocolCustomCommands.h"
#include "internal/Tools.h"

//! number of bytes always present in response (command + payload length + checksum)
#define BASE_LENGTH (sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t))

usbProtocolMessageFrame_t *createMessageFrame(uint8_t command, size_t payloadLength,
                                              uint8_t *payload) {
    usbProtocolMessageFrame_t *msg = malloc(sizeof(usbProtocolMessage_t));
    msg->length = BASE_LENGTH + payloadLength;

    uint32_t payloadLengthData = __builtin_bswap32(payloadLength);

    msg->data = calloc(1, msg->length);
    msg->data[0] = command;
    memcpy(&(msg->data[1]), &payloadLengthData, sizeof(uint32_t));
    memcpy(&(msg->data[5]), payload, payloadLength);
    msg->data[msg->length - 1] = getChecksum(2, msg->data, msg->length - 1);

    return msg;
}
void freeMessageFrame(usbProtocolMessageFrame_t *buffer) {
    free(buffer->data);
    free(buffer);
}
bool waitForAcknowledgement(void) {
    usbProtocolMessage_t ack;
    usbProtocolReadMessage(&ack);
    return ack.command == 0x01;
}
uint32_t convertBytes(const uint8_t data[4]) {
    return data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
}

void readBytes(uint8_t *data, size_t numberOfBytes) {
    if (readHandle(data, numberOfBytes) != USB_PROTOCOL_OKAY) {
        Throw(USB_PROTOCOL_ERROR_READ_FAILED);
    }
}

bool checksumPassed(uint8_t expectedChecksum, int numberOfArguments, ...) {
    va_list data;
    va_start(data, numberOfArguments);
    uint8_t actualChecksum = calculateChecksum(numberOfArguments, data);
    va_end(data);

    return (actualChecksum == expectedChecksum);
}
uint8_t calculateChecksum(int numberOfArguments, va_list data) {
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
uint8_t getChecksum(int numberOfArguments, ...) {
    va_list data;
    va_start(data, numberOfArguments);
    uint8_t actualChecksum = calculateChecksum(numberOfArguments, data);
    va_end(data);

    return actualChecksum;
}
