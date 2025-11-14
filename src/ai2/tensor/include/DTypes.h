#ifndef ELASTIC_AI_RUNTIME_ENV5_DTYPES_H
#define ELASTIC_AI_RUNTIME_ENV5_DTYPES_H

#include <stddef.h>
#include <stdint.h>

int32_t readBytesAsInt32(uint8_t *bytes);
int32_t readNumberOfBytesAsInt32(uint8_t *data, size_t numberOfBytes);
void readBytesAsInt32Array(size_t numberOfValues, uint8_t *bytes, int32_t *outputArray);

float readBytesAsFloat(uint8_t *bytes);
void readBytesAsFloatArray(size_t numberOfValues, uint8_t *bytes, float *outputArray);


void writeInt32ToByteArray(int32_t value, uint8_t *bytes);
void writeInt32ArrayToByteArray(size_t numberOfValues, int32_t *valueArray, uint8_t *bytes);

void writeFloatToByteArray(float value, uint8_t *bytes);
void writeFloatArrayToByteArray(size_t numberOfValues, float *valueArray, uint8_t *bytes);


#endif // ELASTIC_AI_RUNTIME_ENV5_DTYPES_H
