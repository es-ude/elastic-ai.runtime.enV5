#include "Add.h"
#include "Tensor.h"
#include "DTypes.h"

#include <stdlib.h>
#include <string.h>

size_t addInstructionCounter = 0;

float addFloats(float a, float b) {
    ++addInstructionCounter;
    return a + b;
}

int32_t addInt32s(int32_t a, int32_t b) {
    ++addInstructionCounter;
    return a + b;
}

size_t getAddInstructionCounter() {
    return addInstructionCounter;
}

void addFloatTensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(aTensor);

    size_t bytesPerElement = sizeof(float);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t byteIndex = elementIndex * bytesPerElement;

        float a = readBytesAsFloat(&aTensor->data[byteIndex]);
        float b = readBytesAsFloat(&bTensor->data[byteIndex]);
        float output = addFloats(a, b);
        memcpy(&outputTensor[byteIndex], &output, sizeof(float));
    }
}

void addFloatTensors_(tensor_t *aTensor, tensor_t *bTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(aTensor);

    size_t bytesPerElement = sizeof(float);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t byteIndex = elementIndex * bytesPerElement;

        float a = readBytesAsFloat(&aTensor->data[byteIndex]);
        float b = readBytesAsFloat(&bTensor->data[byteIndex]);
        float output = addFloats(a, b);
        memcpy(&aTensor[byteIndex], &output, sizeof(float));
    }
}

void addFloatToTensor(tensor_t *tensor, float f) {
    size_t numberOfElements = calcNumberOfElementsByTensor(tensor);

    size_t bytesPerElement = sizeof(float);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t byteIndex = elementIndex * bytesPerElement;

        float a = readBytesAsFloat(&tensor->data[byteIndex]);
        float output = addFloats(a, f);
        memcpy(&tensor[byteIndex], &output, sizeof(float));
    }
}

void addInt32Tensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {

    size_t numberOfElements = calcNumberOfElementsByTensor(aTensor);
    size_t bytesPerElement = sizeof(int32_t);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t byteIndex = elementIndex * bytesPerElement;

        int32_t a = readBytesAsInt32(&aTensor->data[byteIndex]);
        int32_t b = readBytesAsInt32(&bTensor->data[byteIndex]);
        int32_t output = addInt32s(a, b);
        memcpy(&outputTensor[byteIndex], &output, sizeof(int32_t));
    }
}

void addInt32Tensors_(tensor_t *aTensor, tensor_t *bTensor) {

    size_t numberOfElements = calcNumberOfElementsByTensor(aTensor);
    size_t bytesPerElement = sizeof(int32_t);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t byteIndex = elementIndex * bytesPerElement;

        int32_t a = readBytesAsInt32(&aTensor->data[byteIndex]);
        int32_t b = readBytesAsInt32(&bTensor->data[byteIndex]);
        int32_t output = addInt32s(a, b);
        memcpy(&aTensor[byteIndex], &output, sizeof(int32_t));
    }
}

void addInt32ToTensor(tensor_t *tensor, int32_t x) {

    size_t numberOfElements = calcNumberOfElementsByTensor(tensor);
    size_t bytesPerElement = sizeof(int32_t);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t byteIndex = elementIndex * bytesPerElement;

        int32_t a = readBytesAsInt32(&tensor->data[byteIndex]);
        int32_t output = addInt32s(a, x);
        memcpy(&tensor[byteIndex], &output, sizeof(int32_t));
    }
}
