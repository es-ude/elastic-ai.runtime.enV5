#define SOURCE_FILE "USB-PROTOCOL"

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include "CException.h"

#include "DefaultCommands.h"
#include "include/UsbProtocol.h"

/* region VARIABLES */

/*!
 * @brief buffer storing received command and payload to transfer data to handle function
 */
typedef struct receivedCommand {
    uint8_t command;
    uint8_t *payload;
    size_t payloadLength;
} receivedCommand_t;

static usbProtocolReadCommand readHandle = NULL;
static usbProtocolSendData sendHandle = NULL;
static volatile usbProtocolCommandHandle commands[UINT8_C(0xFF)] = {0};

/* endregion VARIABLES */

/* region INTERNAL FUNCTIONS */

//! @brief check id is between 128 and 255 else @throws USB_PROTOCOL_INVALID_ID
static void checkIdentifierIsInUserRange(uint8_t identifier) {
    if (identifier > UINT8_MAX || identifier < ((UINT8_MAX / 2) + 1)) {
        Throw(USB_PROTOCOL_ERROR_INVALID_ID);
    }
}
//! @brief check handle is not NULL else @throws USB_PROTOCOL_ERROR_NULL_POINTER
static void checkHandlePointerIsNotNull(usbProtocolCommandHandle handle) {
    if (handle == NULL) {
        Throw(USB_PROTOCOL_ERROR_NULL_POINTER);
    }
}
//! @brief check read/send handle are initialized else @throws USB_PROTOCOL_ERROR_NOT_INITIALIZED
static void checkReadAndSendAreSet(void) {
    if (readHandle == NULL | sendHandle == NULL) {
        throws(USB_PROTOCOL_ERROR_NOT_INITIALIZED);
    }
}

//! @brief set command pointer to given function pointers
static void addCommand(uint8_t command, usbProtocolCommandHandle handle) {
    commands[command] = handle;
}
//! @brief set command pointer to NULL
static void removeCommand(uint8_t command) {
    commands[command] = NULL;
}

//! @brief calculate XOR based checksum of given byte arrays
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
//! @brief method to get checksum of arbitrary number of byte arrays
static uint8_t getChecksum(int numberOfArguments, ...) {
    va_list data;
    va_start(data, numberOfArguments);
    uint8_t actualChecksum = calculateChecksum(numberOfArguments, data);
    va_end(data);

    return actualChecksum;
}
//! @brief calculate checksum and evaluate if it matches the expected
static bool checksumPassed(uint8_t expectedChecksum, int numberOfArguments, ...) {
    va_list data;
    va_start(data, numberOfArguments);
    uint8_t actualChecksum = calculateChecksum(numberOfArguments, data);
    va_end(data);

    return (actualChecksum != expectedChecksum);
}

//! read arbitrary number of bytes with read-handle
static void readBytes(uint8_t *data, size_t numberOfBytes) {
    for (size_t index = 0; index < numberOfBytes; index++) {
        while (readHandle(&data[index]) != USB_PROTOCOL_OKAY) {}
    }
}
//! read checksum with read-handle
static uint8_t readChecksum(void) {
    uint8_t checksum = 0;
    while (readHandle(&checksum) != USB_PROTOCOL_OKAY) {}
    return checksum;
}

//! send ACK
static void sendAck(void) {
    uint8_t ack[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    sendHandle(ack, sizeof(ack));
}
//! send NACK
static void sendNack(void) {
    uint8_t ack[6] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x01};
    ack[5] = getChecksum(2, ack, 5);
    sendHandle(ack, sizeof(ack));

    Throw(USB_PROTOCOL_ERROR_CHECKSUM_FAILED);
}

//! clean receivedCommand_t buffer
static void cleanDataBuffer(receivedCommand_t *buffer) {
    free(buffer->payload);
    free(buffer);
}

/* endregion INTERNAL FUNCTIONS */

/* region PUBLIC FUNCTIONS */

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

void usbProtocolRegisterCommand(size_t identifier, usbProtocolCommandHandle command) {
    checkHandlePointerIsNotNull(command);
    checkIdentifierIsInUserRange(identifier);
    addCommand(identifier, command);
}
void usbProtocolUnregisterCommand(size_t identifier) {
    checkIdentifierIsInUserRange(identifier);
    removeCommand(identifier);
}

usbProtocolReceiveBuffer usbProtocolWaitForCommand(void) {
    checkReadAndSendAreSet();

    uint8_t rawCommand[5];
    readBytes(rawCommand, 5);

    receivedCommand_t *received = calloc(1, sizeof(receivedCommand_t));
    received->command = rawCommand[0];
    received->payloadLength =
        (uint32_t)(rawCommand[1] << 24 | rawCommand[2] << 16 | rawCommand[3] << 8 | rawCommand[4]);

    if (received->payloadLength != 0) {
        received->payload = calloc(received->payloadLength, sizeof(uint8_t));
        readBytes(received->payload, received->payloadLength);

        checksumPassed(readChecksum(), 4, rawCommand, 5, received->payload, received->payloadLength)
            ? sendAck()
            : sendNack();
    } else {
        checksumPassed(readChecksum(), 2, rawCommand, 5) ? sendAck() : sendNack();
    }

    return received;
}
void usbProtocolHandleCommand(usbProtocolReceiveBuffer buffer) {
    usbProtocolCommandHandle handle = commands[((receivedCommand_t *)buffer)->command];
    checkHandlePointerIsNotNull(handle);
    CEXCEPTION_T exception;
    Try {
        handle(((receivedCommand_t *)buffer)->payload, ((receivedCommand_t *)buffer)->payloadLength,
               sendHandle);
        cleanDataBuffer(((receivedCommand_t *)buffer));
    }
    Catch(exception) {
        cleanDataBuffer(((receivedCommand_t *)buffer));
        Throw(exception);
    }
}

/* endregion PUBLIC FUNCTIONS */
