#define SOURCE_FILE "USB-PROTOCOL_DEFAULT_COMMANDS"

#include "DefaultCommands.h"

void sendDeviceId(__attribute__((unused)) uint8_t *data, __attribute__((unused)) size_t length,
                  usbProtocolSendData sendFunction) {
    // TODO: implement
}

void sendDataToRam(uint8_t *data, size_t length, usbProtocolSendData sendFunction) {
    // TODO: implement
}

void readDataFromRam(__attribute__((unused)) uint8_t *data, __attribute__((unused)) size_t length,
                     usbProtocolSendData sendFunction) {
    // TODO: implement
}

void sendDataToFlash(uint8_t *data, size_t length, usbProtocolSendData sendFunction) {
    // TODO: implement
}

void readDataFromFlash(__attribute__((unused)) uint8_t *data, __attribute__((unused)) size_t length,
                       usbProtocolSendData sendFunction) {
    // TODO: implement
}

void runInferenceWithRamData(__attribute__((unused)) uint8_t *data,
                             __attribute__((unused)) size_t length,
                             usbProtocolSendData sendFunction) {
    // TODO: implement
}

void runInferenceWithFlashData(__attribute__((unused)) uint8_t *data,
                               __attribute__((unused)) size_t length,
                               usbProtocolSendData sendFunction) {
    // TODO: implement
}

void runTrainingWithRamData(__attribute__((unused)) uint8_t *data,
                            __attribute__((unused)) size_t length,
                            usbProtocolSendData sendFunction) {
    // TODO: implement
}

void runTrainingWithFlashData(__attribute__((unused)) uint8_t *data,
                              __attribute__((unused)) size_t length,
                              usbProtocolSendData sendFunction) {
    // TODO: implement
}
