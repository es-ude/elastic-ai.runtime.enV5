#define SOURCE_FILE "USB-PROTOCOL_DEFAULT_COMMANDS"

#include <string.h>

#include "EnV5HwConfiguration.h"
#include "EnV5HwController.h"
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
    uint32_t payload = __builtin_bswap32(FLASH_BYTES_PER_PAGE);
    uint32_t payloadBytes = sizeof(payload);

    uint8_t response[BASE_LENGTH + sizeof(uint32_t)];
    response[0] = 3;                                           // set command ID
    memcpy(&response[1], &payloadBytes, sizeof(payloadBytes)); // set payload size
    memcpy(&response[5], &payload, sizeof(uint32_t));          // set payload (chunk size)
    response[9] = getChecksum(2, &response, 9);                // set checksum

    sendHandle(response, sizeof(response));
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
    env5HwControllerLedsAllOff();

    if (*data & 0x01) {
        gpioSetPin(LED0_GPIO, GPIO_PIN_HIGH);
    }
    if (*data & 0x02) {
        gpioSetPin(LED1_GPIO, GPIO_PIN_HIGH);
    }
    if (*data & 0x04) {
        gpioSetPin(LED2_GPIO, GPIO_PIN_HIGH);
    }
}

void runInference(const uint8_t *data, size_t length) {
    // TODO: implement
}
