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

static uint8_t getChecksum(const uint8_t *array, size_t length) {
    uint8_t checksum = 0;

    for (size_t index = 0; index < length; index++) {
        checksum ^= array[index];
    }

    return checksum;
}
uint8_t calculateChecksum(usbProtocolMessage_t *message) {
    uint8_t checksum = message->command;
    uint8_t payloadLengthBytes[4];
    convertUint32ToByteArray(message->payloadLength, payloadLengthBytes);
    checksum ^= getChecksum(payloadLengthBytes, 4);
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
    usbProtocolMessageFrame_t *msg = malloc(sizeof(usbProtocolMessageFrame_t));
    msg->length = BASE_LENGTH + payloadLength;

    uint8_t payloadLengthBA[4];
    convertUint32ToByteArray(payloadLength, payloadLengthBA);

    msg->data = calloc(msg->length, sizeof(uint8_t));
    memcpy(msg->data, &command, 1);
    memcpy(&(msg->data[1]), payloadLengthBA, sizeof(payloadLengthBA));
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
    usbProtocolReadHandle(ack, 6);

    return ack[0] == 0x01;
}

void convertUint32ToByteArray(uint32_t in, uint8_t out[4]) {
    uint32_t bigEndianIn = __builtin_bswap32(in);
    memcpy(out, &bigEndianIn, 4);
}
uint32_t convertByteArrayToUint32(const uint8_t in[4]) {
    return in[0] << 24 | in[1] << 16 | in[2] << 8 | in[3];
}

void readBytes(uint8_t *data, size_t numberOfBytes) {
    if (usbProtocolReadHandle(data, numberOfBytes) != USB_PROTOCOL_OKAY) {
        Throw(USB_PROTOCOL_ERROR_READ_FAILED);
    }
}

void sendAck(void) {
    uint8_t ack[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    usbProtocolSendHandle(ack, sizeof(ack));
}
void sendNack(void) {
    uint8_t nack[6] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x01};
    usbProtocolSendHandle(nack, sizeof(nack));
}
