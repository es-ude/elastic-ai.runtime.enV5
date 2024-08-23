#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "CException.h"

#include "UsbProtocolBase.h"
#include "UsbProtocolCustomCommands.h"
#include "internal/Tools.h"

typedef union u32_u8 {
    uint32_t data;
    uint8_t array[4];
} u32_u8_t;

//! number of bytes always present in response (command + payload length + checksum)
#define BASE_LENGTH (sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t))

static uint8_t getChecksum(const uint8_t *array, size_t length) {
    uint8_t checksum = 0;

    for (size_t index = 0; index < length; index++) {
        checksum ^= array[index];
    }

    return checksum;
}
uint8_t calculateChecksum(usbProtocolMessage_t *message) {
    uint8_t checksum = message->command;
    u32_u8_t payloadData = {.data = __builtin_bswap32(message->payloadLength)};
    checksum ^= getChecksum(payloadData.array, 4);
    if (message->payloadLength > 0) {
        checksum ^= getChecksum(message->payload, message->payloadLength);
    }

    return checksum;
}
bool checksumPassed(uint8_t expectedChecksum, usbProtocolMessage_t *message) {
    uint8_t actualChecksum = calculateChecksum(message);
    return (actualChecksum == expectedChecksum);
}

usbProtocolMessageFrame_t *createMessageFrame(uint8_t command, size_t payloadLength,
                                              uint8_t *payload, uint8_t checksum) {
    usbProtocolMessageFrame_t *msg = malloc(sizeof(usbProtocolMessage_t));
    msg->length = BASE_LENGTH + payloadLength;

    u32_u8_t payloadLengthData = {.data = __builtin_bswap32(payloadLength)};

    msg->data = calloc(1, msg->length);
    memcpy(msg->data, &command, 1);
    memcpy(&(msg->data[1]), payloadLengthData.array, sizeof(payloadLengthData));
    memcpy(&(msg->data[5]), payload, payloadLength);
    memcpy(&(msg->data[msg->length - 1]), &checksum, 1);

    return msg;
}
void freeMessageFrame(usbProtocolMessageFrame_t *buffer) {
    free(buffer->data);
    free(buffer);
}
bool waitForAcknowledgement(void) {
    uint8_t ack[6];
    readHandle(ack, 6);

    return ack[0] == 0x01;
}
uint32_t convertBytes(const uint8_t data[4]) {
    return data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
}

void readBytes(uint8_t *data, size_t numberOfBytes) {
    if (readHandle(data, numberOfBytes) != USB_PROTOCOL_OKAY) {
        Throw(USB_PROTOCOL_ERROR_READ_FAILED);
    }
}

void sendAck(void) {
    uint8_t ack[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    sendHandle(ack, sizeof(ack));
}
void sendNack(void) {
    uint8_t nack[6] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x01};
    sendHandle(nack, sizeof(nack));
}
