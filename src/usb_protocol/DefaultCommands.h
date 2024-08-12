#ifndef ENV5_DEFAULT_COMMANDS_HEADER
#define ENV5_DEFAULT_COMMANDS_HEADER

#include <stddef.h>
#include <stdint.h>

#include "include/UsbProtocol.h"

void sendDeviceId(uint8_t *data, size_t length, usbProtocolSendData sendFunction);
void sendDataToRam(uint8_t *data, size_t length, usbProtocolSendData sendFunction);
void readDataFromRam(uint8_t *data, size_t length, usbProtocolSendData sendFunction);
void sendDataToFlash(uint8_t *data, size_t length, usbProtocolSendData sendFunction);
void readDataFromFlash(uint8_t *data, size_t length, usbProtocolSendData sendFunction);
void runInferenceWithRamData(uint8_t *data, size_t length, usbProtocolSendData sendFunction);
void runInferenceWithFlashData(uint8_t *data, size_t length, usbProtocolSendData sendFunction);
void runTrainingWithRamData(uint8_t *data, size_t length, usbProtocolSendData sendFunction);
void runTrainingWithFlashData(uint8_t *data, size_t length, usbProtocolSendData sendFunction);

#endif // ENV5_DEFAULT_COMMANDS_HEADER
