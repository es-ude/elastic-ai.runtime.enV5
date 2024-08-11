#ifndef ENV5_DEFAULT_COMMANDS_HEADER
#define ENV5_DEFAULT_COMMANDS_HEADER

#include <stddef.h>
#include <stdint.h>

void sendDeviceId(__attribute__((unused)) uint8_t *data, __attribute__((unused)) size_t length);
void sendDataToRam(uint8_t *data, size_t length);
void readDataFromRam(uint8_t *data, size_t length);
void sendDataToFlash(uint8_t *data, size_t length);
void readDataFromFlash(uint8_t *data, size_t length);
void runInferenceWithRamData(uint8_t *data, size_t length);
void runInferenceWithFlashData(uint8_t *data, size_t length);
void runTrainingWithRamData(uint8_t *data, size_t length);
void runTrainingWithFlashData(uint8_t *data, size_t length);

#endif // ENV5_DEFAULT_COMMANDS_HEADER
