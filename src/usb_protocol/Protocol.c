#define SOURCE_FILE "USB-PROTOCOL"

#include <stddef.h>
#include <stdlib.h>

#include "CException.h"

#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
#include "Gpio.h"
#include "Sleep.h"
#include "UsbProtocol.h"
#include "UsbProtocolTypedefs.h"
#include "internal/DefaultCommands.h"
#include "internal/Tools.h"

/* region VARIABLES */

/*!
 * @brief buffer storing received command and payload to transfer data to handle function
 */
typedef struct receivedCommand {
    uint8_t command;
    uint8_t *payload;
    size_t payloadLength;
} receivedCommand_t;

usbProtocolReadData readHandle = NULL;
usbProtocolSendData sendHandle = NULL;
static usbProtocolCommandHandle commands[UINT8_C(0xFF)];

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
        Throw(USB_PROTOCOL_ERROR_HANDLE_NOT_SET);
    }
}
//! @brief check read/send handle are initialized else @throws USB_PROTOCOL_ERROR_NOT_INITIALIZED
static void checkReadAndSendAreSet(void) {
    if (readHandle == NULL | sendHandle == NULL) {
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
    uint8_t nack[6] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x01};
    sendHandle(nack, sizeof(nack));

    Throw(USB_PROTOCOL_ERROR_CHECKSUM_FAILED);
}

//! clean receivedCommand_t buffer
static void cleanDataBuffer(receivedCommand_t *buffer) {
    if (buffer == NULL) {
        return;
    }
    if (buffer->payload != NULL) {
        free(buffer->payload);
    }
    free(buffer);
}

/* endregion INTERNAL FUNCTIONS */

/* region PUBLIC FUNCTIONS */

static void addDefaultFunctions(void) {
    addCommand(2, &readSkeletonId);
    addCommand(3, &getChunkSize);
    addCommand(4, &sendDataToFlash);
    addCommand(5, &readDataFromFlash);
    addCommand(6, &setFpgaPower);
    addCommand(6, &setFpgaLeds);
    addCommand(7, &setMcuLeds);
    addCommand(8, &runInference);
}
void usbProtocolInit(usbProtocolReadData readFunction, usbProtocolSendData sendFunction) {
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

static void blinkLED2(uint8_t cmd) {
    env5HwControllerLedsAllOff();
    gpioSetPin(LED1_GPIO, GPIO_PIN_HIGH);
    sleep_for_ms(2000);
    env5HwControllerLedsAllOff();

    for (uint8_t index = 0; index < cmd; index++) {
        sleep_for_ms(500);
        gpioSetPin(LED1_GPIO, GPIO_PIN_HIGH);
        sleep_for_ms(500);
        gpioSetPin(LED1_GPIO, GPIO_PIN_LOW);
    }
}
void usbProtocolHandleCommand(usbProtocolReceiveBuffer buffer) {
    receivedCommand_t *input = buffer;
    uint8_t command = input->command;
    blinkLED2(command);

    usbProtocolCommandHandle handle = commands[command];
    checkHandlePointerIsNotNull(handle);

    CEXCEPTION_T exception;
    Try {
        handle(((receivedCommand_t *)buffer)->payload,
               ((receivedCommand_t *)buffer)->payloadLength);
        cleanDataBuffer(input);
    }
    Catch(exception) {
        cleanDataBuffer(input);
        Throw(exception);
    }
}

/* endregion PUBLIC FUNCTIONS */
