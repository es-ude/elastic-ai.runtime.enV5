#ifndef ENV5_DEFAULT_COMMANDS_HEADER
#define ENV5_DEFAULT_COMMANDS_HEADER

#include <stddef.h>
#include <stdint.h>

void readSkeletonId(const uint8_t *data, size_t length);
void getChunkSize(const uint8_t *data, size_t length);
void sendDataToFlash(const uint8_t *data, size_t length);
void readDataFromFlash(const uint8_t *data, size_t length);
void setFpgaPower(const uint8_t *data, size_t length);
void setFpgaLeds(const uint8_t *data, size_t length);
void setMcuLeds(const uint8_t *data, size_t length);
void runInference(const uint8_t *data, size_t length);

#endif // ENV5_DEFAULT_COMMANDS_HEADER
