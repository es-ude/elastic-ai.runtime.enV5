#define SOURCE_FILE "USB-PROTOCOL"

#include <stddef.h>
#include <stdlib.h>

#include "CException.h"

#include "Sleep.h"
#include "UsbProtocolBase.h"
#include "UsbProtocolCustomCommands.h"
#include "internal/Tools.h"

/* region TYPEDEFS/VARIABLES/DEFINES */

/*!
 * @brief buffer storing received command and payload to transfer data to handle function
 */
typedef struct receivedCommand {
    uint8_t command;
    uint8_t *payload;
    size_t payloadLength;
} receivedCommand_t;

usbProtocolReadData usbProtocolReadHandle = NULL;
usbProtocolSendData usbProtocolSendHandle = NULL;
static usbProtocolCommandHandle commands[UINT8_C(0xFF)];

flashConfiguration_t *usbProtocolFlashConfig;

/* endregion TYPEDEFS/VARIABLES/DEFINES */

/* region INTERNAL FUNCTIONS */

//! @brief check id is between 128 and 255 else @throws USB_PROTOCOL_INVALID_ID
static void checkIdentifierIsInUserRange(uint8_t identifier) {
    if (identifier > UINT8_MAX || identifier < ((UINT8_MAX / 2) + 1)) {
        Throw(USB_PROTOCOL_ERROR_INVALID_ID);
    }
}

//! @brief check handle is not NULL else @throws USB_PROTOCOL_ERROR_HANDLE_NOT_SET
static void checkHandlePointerIsNotNull(usbProtocolCommandHandle handle) {
    if (handle == NULL) {
        Throw(USB_PROTOCOL_ERROR_HANDLE_NOT_SET);
    }
}

//! @brief check pointer is not NULL else @throws USB_PROTOCOL_ERROR_NULL_POINTER
static void checkPointerIsNotNull(void *pointer) {
    if (pointer == NULL) {
        Throw(USB_PROTOCOL_ERROR_NULL_POINTER);
    }
}

//! @brief check read/send handle are initialized else @throws USB_PROTOCOL_ERROR_NOT_INITIALIZED
static void checkReadAndSendHandleAreAvailable(void) {
    if (usbProtocolReadHandle == NULL | usbProtocolSendHandle == NULL) {
        Throw(USB_PROTOCOL_ERROR_NOT_INITIALIZED);
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

//! read checksum with read-handle
static uint8_t readChecksum(void) {
    uint8_t checksum = 0;
    if (usbProtocolReadHandle(&checksum, 1) != USB_PROTOCOL_OKAY) {
        Throw(USB_PROTOCOL_ERROR_READ_FAILED);
    }
    return checksum;
}

static void cleanMessageBuffer(usbProtocolMessage_t *message) {
    if (message != NULL) {
        if (message->payload != NULL) {
            free(message->payload);
        }

        free(message);
    }
}

/* endregion INTERNAL FUNCTIONS */

/* region PUBLIC FUNCTIONS */

void usbProtocolInit(
    usbProtocolReadData readFunction,
    usbProtocolSendData sendFunction,
    usbDefaultCommands *defaultCommands) {
    if (readFunction == NULL || sendFunction == NULL) {
        Throw(USB_PROTOCOL_ERROR_NULL_POINTER);
    }

    usbProtocolReadHandle = readFunction;
    usbProtocolSendHandle = sendFunction;

    defaultCommands();
}

void setupFlashForUsbProtocol(flashConfiguration_t *flashConfiguration) {
    usbProtocolFlashConfig = flashConfiguration;
}

bool usbProtocolSendMessage(usbProtocolMessage_t *message) {
    usbProtocolMessageFrame_t *frame = createMessageFrame(
        message->command, message->payloadLength, message->payload, calculateChecksum(message));
    usbProtocolSendHandle(frame->data, frame->length);
    freeMessageFrame(frame);

    return waitForAcknowledgement();
}

bool usbProtocolReadMessage(usbProtocolMessage_t *message) {
    checkPointerIsNotNull(message);

    uint8_t buffer[5];
    readBytes(buffer, 5);
    message->command = buffer[0];
    message->payloadLength = convertByteArrayToUint32(&buffer[1]);

    if (message->payloadLength > 0) {
        message->payload = calloc(1, message->payloadLength);
        readBytes(message->payload, message->payloadLength);
    }

    if (!checksumPassed(readChecksum(), message)) {
        sendNack();
        return false;
    } else {
        sendAck();
        return true;
    }
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
    checkReadAndSendHandleAreAvailable();

    usbProtocolMessage_t *message = calloc(1, sizeof(usbProtocolMessage_t));
    if (!usbProtocolReadMessage(message)) {
        Throw(USB_PROTOCOL_ERROR_CHECKSUM_FAILED);
    }

    return message;
}

void usbProtocolHandleCommand(usbProtocolReceiveBuffer buffer) {
    usbProtocolMessage_t *message = buffer;

    usbProtocolCommandHandle handle = commands[message->command];
    checkHandlePointerIsNotNull(handle);

    CEXCEPTION_T exception;
    Try {
        handle(message->payload, message->payloadLength);
        cleanMessageBuffer(message);
    }
    Catch(exception) {
        cleanMessageBuffer(message);
        Throw(exception);
    }
}
/* endregion PUBLIC FUNCTIONS */
