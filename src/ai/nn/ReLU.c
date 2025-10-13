#define SOURCE_FILE "AI_NN_RELU"

#include "ReLU.h"
#include "Quantization.h"

#include <Float16.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t zero = 0;

ReLUConfig_t *initReLUConfig() {
    ReLUConfig_t *config = calloc(1, sizeof(ReLUConfig_t));
    return config;
}

layerForward_t *initReLULayerForward() {
    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->type = RELU;
    layerForward->config = initReLUConfig(0);
    return layerForward;
}

layerForwardBackward_t *initReLULayerForwardBackward() {
    layerForwardBackward_t *layerForwardBackward = calloc(1, sizeof(layerForwardBackward_t));
    layerForwardBackward->type = RELU;
    layerForwardBackward->config = initReLUConfig(0);
    return layerForwardBackward;
}

void reluForwardFloat32(uint8_t *rawBytes, size_t byteIndex, uint8_t *outputData,
                        size_t bytesPerElement) {
    float32_t currentElement = readBytesAsFloat32(
        &rawBytes[byteIndex]);
    if (currentElement < 0.f) {
        memcpy(&outputData[byteIndex], &zero, bytesPerElement);
    } else {
        memcpy(&outputData[byteIndex], &rawBytes[byteIndex], bytesPerElement);
    }
}

void reluForwardFloat64(uint8_t *rawBytes, size_t byteIndex, uint8_t *outputData,
                        size_t bytesPerElement) {
    float64_t currentElement = readBytesAsFloat64(
        &rawBytes[byteIndex]);
    if (currentElement < 0.f) {
        memcpy(&outputData[byteIndex], &zero, bytesPerElement);
    } else {
        memcpy(&outputData[byteIndex], &rawBytes[byteIndex], bytesPerElement);
    }
}

reluForward_t reluForwardTable[] = {
    [FLOAT32_Q] = reluForwardFloat32,
    [FLOAT64_Q] = reluForwardFloat64
};

void reluBackwardFloat32(uint8_t *rawBytes, uint8_t *gradBytes, size_t byteIndex,
                         uint8_t *outputData,
                         size_t bytesPerElement) {
    float32_t currentElement = readBytesAsFloat32(&rawBytes[byteIndex]);
    if (currentElement <= 0.0f) {
        memcpy(&outputData[byteIndex], &zero, bytesPerElement);
    } else {
        memcpy(&outputData[byteIndex], &gradBytes[byteIndex], bytesPerElement);
    }
}

void reluBackwardFloat64(uint8_t *rawBytes, uint8_t *gradBytes, size_t byteIndex,
                         uint8_t *outputData,
                         size_t bytesPerElement) {
    float64_t currentElement = readBytesAsFloat64(&rawBytes[byteIndex]);
    if (currentElement <= 0.0f) {
        memcpy(&outputData[byteIndex], &zero, bytesPerElement);
    } else {
        memcpy(&outputData[byteIndex], &gradBytes[byteIndex], bytesPerElement);
    }
}

reluBackward_t reluBackwardTable[] = {
    [FLOAT32_Q] = reluBackwardFloat32,
    [FLOAT64_Q] = reluBackwardFloat64
};

void freeReLUForward(layerForward_t *layer) {}

void freeReLUBackward(layerForwardBackward_t *layer) {}

qTensor_t *ReLUForward(void *config, qTensor_t *inputQTensor, quantization_t *outputQuantization) {
    size_t numberOfDims = inputQTensor->numberOfDimensions;
    size_t *dims = calloc(numberOfDims, sizeof(size_t));
    memcpy(dims, inputQTensor->dimensions, numberOfDims * sizeof(size_t));

    size_t totalNumberOfElements = calcTotalNumberOfElementsByTensor(inputQTensor);
    size_t bytesPerElement = calcBytesPerElement(inputQTensor->quantization);

    uint8_t *outputData = calloc(totalNumberOfElements, bytesPerElement);

    if (inputQTensor->quantization->type == outputQuantization->type) {
        reluForward_t reluForward = reluForwardTable[inputQTensor->quantization->type];

        for (size_t elementIndex = 0; elementIndex < totalNumberOfElements; elementIndex++) {
            size_t byteIndex = elementIndex * bytesPerElement;
            reluForward(inputQTensor->data, byteIndex, outputData, bytesPerElement);
        }
    } else {
        printf("Error: Mismatched Quantization not implemented yet");
        return NULL;
    }

    qTensor_t *outputQTensor = initQTensor(outputData, numberOfDims, dims, outputQuantization);

    free(dims);
    free(outputData);

    return outputQTensor;
}

qTensor_t *ReLUBackward(void *config, qTensor_t *gradQTensor, qTensor_t *inputQTensor) {
    size_t numberOfDims = inputQTensor->numberOfDimensions;
    size_t *dims = calloc(numberOfDims, sizeof(size_t));
    memcpy(dims, inputQTensor->dimensions, numberOfDims);

    size_t bytesPerElement = calcBytesPerElement(inputQTensor->quantization);
    size_t totalNumberOfElements = calcTotalNumberOfElementsByTensor(inputQTensor);
    uint8_t *outputData = calloc(totalNumberOfElements, bytesPerElement);

    if (inputQTensor->quantization->type == gradQTensor->quantization->type) {
        reluBackward_t reluBackward = reluBackwardTable[inputQTensor->quantization->type];
        for (size_t i = 0; i < totalNumberOfElements; i++) {
            size_t byteIndex = i * bytesPerElement;
            reluBackward(inputQTensor->data, gradQTensor->data, byteIndex, outputData,
                         bytesPerElement);
        }
    } else {
        printf("Error: Mismatched Quantization not implemented yet");
        return NULL;
    }

    qTensor_t *outputQTensor = initQTensor(outputData, numberOfDims, dims,
                                           inputQTensor->quantization);
    free(dims);
    free(outputData);
    return outputQTensor;
}
