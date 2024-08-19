#define SOURCE_FILE "USB-PROTOCOL_DEFAULT_COMMANDS"

#include <string.h>

#include "CException.h"
#include "EnV5HwConfiguration.h"
#include "Gpio.h"
#include "internal/DefaultCommands.h"
#include "internal/Tools.h"

//! number of bytes always present in response (command + payload length + checksum)
#define BASE_LENGTH (sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t))

void readSkeletonId(__attribute__((unused)) const uint8_t *data,
                    __attribute((unused)) size_t length) {
    // TODO: implement
}

void getChunkSize(__attribute__((unused)) const uint8_t *data,
                  __attribute((unused)) size_t length) {
    uint32_t payload = FLASH_BYTES_PER_PAGE;
    payload = __builtin_bswap32(payload);
    uint32_t payloadBytes = sizeof(payload);
    payloadBytes = __builtin_bswap32(payloadBytes);

    uint8_t response[BASE_LENGTH + sizeof(uint32_t)];
    response[0] = 3;                                           // set command ID
    memcpy(&response[1], &payloadBytes, sizeof(payloadBytes)); // set payload size
    memcpy(&response[5], &payload, sizeof(uint32_t));          // set payload (chunk size)
    response[9] = getChecksum(2, &response, 9);                // set checksum

    sendHandle(response, sizeof(response));
    if (waitForAcknowledgement()) {
        Throw(USB_PROTOCOL_ERROR_HANDLE_EXECUTION_FAILED);
    }
}

void sendDataToFlash(const uint8_t *data, size_t length) {
    // TODO: implement
}

void readDataFromFlash(__attribute__((unused)) const uint8_t *data,
                       __attribute((unused)) size_t length) {
    // TODO: implement
}

void setFpgaPower(const uint8_t *data, size_t length) {
    // TODO: implement
}

void setFpgaLeds(const uint8_t *data, size_t length) {
    // TODO: implement
}

void setMcuLeds(const uint8_t *data, __attribute((unused)) size_t length) {
    gpioSetPin(LED0_GPIO, data[0] & 0x01 ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    gpioSetPin(LED1_GPIO, data[0] & 0x02 ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
    gpioSetPin(LED2_GPIO, data[0] & 0x04 ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
}

void runInference(const uint8_t *data, size_t length) {
    // TODO: implement
}
