#include "Comparison.h"
#include "DTypes.h"

#include <stdio.h>

void gteInt32Value(tensor_t *a, int32_t b, int32_t altNumber, tensor_t *result) {
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

void gteInt32Tensor(tensor_t *a, tensor_t *b, int32_t altNumber, tensor_t *result) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(a);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(b);
    if (aNumberOfValues != bNumberOfValues) {
        printf("Error in Comparison of int32 tensors: mismatched number of values\n");
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

void gteFloatValue(tensor_t *a, float b, float altNumber, tensor_t *result) {
    size_t numberOfValues = calcNumberOfElementsByTensor(a);
    float *inputValues = (float *)a->data;
    float *outputValues = (float *)result->data;

    for (size_t i = 0; i < numberOfValues; i++) {
        if (inputValues[i] < b) {
            outputValues[i] = altNumber;
        }
        else {
            outputValues[i] = inputValues[i];
        }
    }
}

void gteFloatTensor(tensor_t *a, tensor_t *b, float altNumber, tensor_t *result) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(a);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(b);
    if (aNumberOfValues != bNumberOfValues) {
        printf("Error in Comparison of float tensors: mismatched number of values\n");
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

void gteSymInt32Zero(tensor_t *a, int32_t altNumber, tensor_t *result) {
    size_t numberOfValues = calcNumberOfElementsByTensor(a);

    int32_t *inputValues = (int32_t *)a->data;
    int32_t *outputValues = (int32_t *)result->data;

    for (size_t i = 0; i < numberOfValues; i++) {
        int32_t currentValue = inputValues[i];
        if (currentValue < 0) {
            currentValue = altNumber;
        }
        outputValues[i] = currentValue;
    }
}

// QUESTION
// TODO
void gteSymInt32Value(tensor_t *a, int32_t b, int32_t altNumber, tensor_t *result) {
    size_t numberOfValues = calcNumberOfElementsByTensor(a);
    int32_t values[numberOfValues];
    readBytesAsInt32Array(numberOfValues, a->data, values);

    symInt32QConfig_t *aSymInt32QC = a->quantization->qConfig;
    float scale = aSymInt32QC->scale;
    float scaledB = (float)b / scale;

    for (size_t i = 0; i < numberOfValues; i++) {
        if ((float)values[i] < scaledB) {
            values[i] = altNumber;
        }
    }

    writeInt32ArrayToByteArray(numberOfValues, values, result->data);
}

void gteSymInt32Tensor(tensor_t *a, tensor_t *b, int32_t altNumber, tensor_t *result) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(a);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(b);
    if (aNumberOfValues != bNumberOfValues) {
        printf("Error in Comparison of float tensors: mismatched number of values\n");
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
