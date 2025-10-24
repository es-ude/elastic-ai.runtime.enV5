#include "Comparison.h"
#include "DTypes.h"

#include <stdio.h>

void GTE_int32Value(tensor_t *a, int32_t b, int32_t altNumber, tensor_t *result) {
    size_t numberOfValues = calcNumberOfElementsByTensor(a);
    int32_t values[numberOfValues];
    readBytesAsInt32Array(numberOfValues, a->data, values);

    for (size_t i = 0; i < numberOfValues; i++) {
        if (values[i] < b) {
            values[i] = altNumber;
        }
    }

    writeInt32ArrayToByteArray(numberOfValues, values, result->data);
}

void GTE_int32Tensor(tensor_t *a, tensor_t *b, int32_t altNumber, tensor_t *result) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(a);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(b);
    if (aNumberOfValues != bNumberOfValues) {
        printf("Error in Comparison of int32 tensors: mismatched number of values");
    }

    int32_t aValues[aNumberOfValues];
    int32_t bValues[bNumberOfValues];
    readBytesAsInt32Array(aNumberOfValues, a->data, aValues);
    readBytesAsInt32Array(bNumberOfValues, b->data, bValues);

    for (size_t i = 0; i < aNumberOfValues; i++) {
        if (aValues[i] < bValues[i]) {
            aValues[i] = altNumber;
        }
    }
    writeInt32ArrayToByteArray(aNumberOfValues, aValues, result->data);
}

void GTE_floatValue(tensor_t *a, float b, float altNumber, tensor_t *result) {
    size_t numberOfValues = calcNumberOfElementsByTensor(a);
    float values[numberOfValues];
    readBytesAsFloatArray(numberOfValues, a->data, values);

    for (size_t i = 0; i < numberOfValues; i++) {
        if (values[i] < b) {
            values[i] = altNumber;
        }
    }
    writeFloatArrayToByteArray(numberOfValues, values, result->data);
}

void GTE_floatTensor(tensor_t *a, tensor_t *b, float altNumber, tensor_t *result) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(a);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(b);
    if (aNumberOfValues != bNumberOfValues) {
        printf("Error in Comparison of float tensors: mismatched number of values");
    }

    float aValues[aNumberOfValues];
    float bValues[bNumberOfValues];
    readBytesAsFloatArray(aNumberOfValues, a->data, aValues);
    readBytesAsFloatArray(bNumberOfValues, b->data, bValues);

    for (size_t i = 0; i < aNumberOfValues; i++) {
        if (aValues[i] < bValues[i]) {
            aValues[i] = altNumber;
        }
    }
    writeFloatArrayToByteArray(aNumberOfValues, aValues, result->data);
}
