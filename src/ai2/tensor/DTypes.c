#include "DTypes.h"
#include "Tensor.h"

#include <stdlib.h>
#include <string.h>

int32_t readBytesAsInt32(uint8_t *bytes) {
    int32_t x;
    memcpy(&x, bytes, sizeof(int32_t));
    return x;
}

int32_t readNumberOfBytesAsInt32(uint8_t *data, size_t numberOfBytes) {
    int32_t output = 0;
    memcpy(&output, data, numberOfBytes);

    return output;
}

void readBytesAsInt32Array(size_t numberOfValues, uint8_t *bytes, int32_t *outputArray) {
    for (size_t i = 0; i < numberOfValues; i++) {
        size_t byteIndex = i * sizeof(int32_t);
        int32_t value = readBytesAsInt32(&bytes[byteIndex]);
        outputArray[i] = value;
    }
}

float readBytesAsFloat(uint8_t *bytes) {
    float x;
    memcpy(&x, bytes, sizeof(float));
    return x;
}

void readBytesAsFloatArray(size_t numberOfValues, uint8_t *bytes, float *outputArray) {
    for (size_t i = 0; i < numberOfValues; i++) {
        size_t byteIndex = i * sizeof(float);
        float value = readBytesAsFloat(&bytes[byteIndex]);
        outputArray[i] = value;
    }
}

void writeInt32ToByteArray(int32_t value, uint8_t *bytes) {
    memcpy(bytes, &value, sizeof(int32_t));
}

void writeInt32ArrayToByteArray(size_t numberOfValues, int32_t *valueArray, uint8_t *bytes) {
    for (size_t i = 0; i < numberOfValues; i++) {
        size_t byteIndex = i * sizeof(int32_t);
        memcpy(&bytes[byteIndex], &valueArray[i], sizeof(int32_t));
    }
}

void writeFloatToByteArray(float value, uint8_t *bytes) {
    memcpy(bytes, &value, sizeof(float));
}

void writeFloatArrayToByteArray(size_t numberOfValues, float *valueArray, uint8_t *bytes) {
    for (size_t i = 0; i < numberOfValues; i++) {
        size_t byteIndex = i * sizeof(float);
        memcpy(&bytes[byteIndex], &valueArray[i], sizeof(float));
    }
}
