#define SOURCE_FILE "AI_NN_LINEAR"

#include "Linear.h"
#include "Quantization.h"

#include <Common.h>
#include <stdlib.h>
#include <string.h>

qTensor_t *initLinearOutputQTensor(linearConfig_t *linearConfig, qTensor_t *inputQTensor,
                                   quantization_t *outputQuantization) {
    size_t totalOutputSize = linearConfig->weight->dataTensor->dimensions[0];

    size_t numberOfDimensions = 2;
    // Needs to change for batch processing
    size_t dimensions[] = {1, totalOutputSize};

    size_t bytesPerElement = calcBytesPerElement(outputQuantization);
    uint8_t *data = calloc(totalOutputSize, bytesPerElement);

    qTensor_t *outputQTensor =
        initQTensor(data, numberOfDimensions, dimensions, outputQuantization);
    return outputQTensor;
}

void linearForwardFloat32(linearConfig_t *linearConfig, qTensor_t *inputQTensor,
                          qTensor_t *outputQTensor, size_t i, size_t inputSize,
                          size_t inputBytesPerElement) {
    float32_t result = 0;
    for (size_t j = 0; j < inputSize; j++) {
        size_t inputByteIndex = j * inputBytesPerElement;

        size_t weightIndex = i * inputSize + j;
        size_t weightBytesPerElement = calcBytesPerElement(
            linearConfig->weight->dataTensor->quantization);
        size_t weightByteIndex = weightIndex * weightBytesPerElement;

        float32_t currentInput = readBytesAsFloat32(&inputQTensor->data[inputByteIndex]);
        float32_t currentWeight = readBytesAsFloat32(
            &linearConfig->weight->dataTensor->data[weightByteIndex]);
        result += currentInput * currentWeight;
    }
    size_t outputBytesPerElement = calcBytesPerElement(outputQTensor->quantization);
    size_t outputByteIndex = i * outputBytesPerElement;
    result += readBytesAsFloat32(&linearConfig->bias->dataTensor->data[outputByteIndex]);
    memcpy(&outputQTensor->data[outputByteIndex], &result, outputBytesPerElement);
}

void linearForwardFloat64(linearConfig_t *linearConfig, qTensor_t *inputQTensor,
                          qTensor_t *outputQTensor, size_t i, size_t inputSize,
                          size_t inputBytesPerElement) {
    float64_t result = 0;
    for (size_t j = 0; j < inputSize; j++) {
        size_t inputByteIndex = j * inputBytesPerElement;

        size_t weightIndex = i * inputSize + j;
        size_t weightBytesPerElement = calcBytesPerElement(
            linearConfig->weight->dataTensor->quantization);
        size_t weightByteIndex = weightIndex * weightBytesPerElement;

        float64_t currentInput = readBytesAsFloat64(&inputQTensor->data[inputByteIndex]);
        float64_t currentWeight = readBytesAsFloat64(
            &linearConfig->weight->dataTensor->data[weightByteIndex]);
        result += currentInput * currentWeight;
    }
    size_t outputBytesPerElement = calcBytesPerElement(outputQTensor->quantization);
    size_t outputByteIndex = i * outputBytesPerElement;
    result += readBytesAsFloat64(&linearConfig->bias->dataTensor->data[outputByteIndex]);
    memcpy(&outputQTensor->data[outputByteIndex], &result, outputBytesPerElement);
}

linearForward_t linearForwardTable[] = {
    [FLOAT32_Q] = linearForwardFloat32,
    [FLOAT64_Q] = linearForwardFloat64
};

qTensor_t *linearForward(void *config, qTensor_t *inputQTensor,
                         quantization_t *outputQuantization) {
    linearConfig_t *linearConfig = config;

    qTensor_t *outputQTensor = initLinearOutputQTensor(linearConfig, inputQTensor,
                                                       outputQuantization);

    size_t outputSize = linearConfig->weight->dataTensor->dimensions[0];
    size_t inputSize = linearConfig->weight->dataTensor->dimensions[1];

    size_t inputBytesPerElement = calcBytesPerElement(inputQTensor->quantization);

    linearForward_t linearForward = linearForwardTable[outputQuantization->type];

    for (size_t i = 0; i < outputSize; i++) {
        linearForward(linearConfig, inputQTensor, outputQTensor, i, inputSize,
                      inputBytesPerElement);
    }

    return outputQTensor;
}

void linearBackwardFloat32() {}

void linearBackwardFloat64() {}

linearBackward_t linearBackwardTable[] = {
    [FLOAT32_Q] = linearBackwardFloat32,
    [FLOAT64_Q] = linearBackwardFloat64
};

qTensor_t *linearBackward(void *config, qTensor_t *lossQTensor, qTensor_t *outputQTensor,
                          quantization_t *outputQuantization) {
    linearConfig_t *linearConfig = config;

    size_t outputSize = linearConfig->weight->dataTensor->dimensions[0];
    size_t inputSize = linearConfig->weight->dataTensor->dimensions[1];

    size_t numberOfDimensions = outputQTensor->numberOfDimensions;
    size_t *dimensions = calloc(numberOfDimensions, sizeof(size_t));
    memcpy(dimensions, outputQTensor->dimensions,
           numberOfDimensions * sizeof(size_t));

    size_t bytesPerElement = calcBytesPerElement(outputQuantization);
    size_t totalNumberOfElements = calcTotalNumberOfElementsByTensor(outputQTensor);
    uint8_t *propagatedLoss = calloc(totalNumberOfElements, bytesPerElement);

    size_t base = bytesPerElement * 8;

    switch (outputQuantization->type) {
    case FLOAT32_Q:
        for (size_t lossIndex = 0; lossIndex < outputSize; lossIndex++) {
            size_t lossByteIndex = lossIndex * base;
            size_t biasByteIndex = lossIndex * base;

            uint8_t *lossAddress = &lossQTensor->data[lossByteIndex];
            float32_t loss = readBytesAsFloat32(lossAddress);

            for (size_t inputIndex = 0; inputIndex < inputSize; inputIndex++) {

                size_t weightIndex = lossIndex * inputSize + inputIndex;
                size_t weightByteIndex = weightIndex * base;

                size_t dataByteIndex = inputIndex * base;
                size_t outputByteIndex = inputIndex * base;

                uint8_t *weightAddress = &linearConfig->weight->dataTensor->data[weightByteIndex];
                float32_t weight = readBytesAsFloat32(weightAddress);

                uint8_t *weightGradAddress = &linearConfig->weight->gradTensor->data[weightByteIndex];
                float32_t weightGrad = readBytesAsFloat32(weightGradAddress);

                uint8_t *propLossAddress = &propagatedLoss[dataByteIndex];
                float32_t propLoss = readBytesAsFloat32(propLossAddress);

                uint8_t *outputAddress = &outputQTensor->data[outputByteIndex];
                float32_t output = readBytesAsFloat32(outputAddress);

                weightGrad += loss * output;
                writeFloat32ToByteArray(weightGrad, weightGradAddress);

                /*linearConfig->weight->gradTensor->data[weightIndex] += lossQTensor->data[lossIndex] *
                    outputQTensor->
                    data[inputIndex];*/

                propLoss += weight * loss;
                writeFloat32ToByteArray(propLoss, &propagatedLoss[dataByteIndex]);

                /*propagatedLoss->data[inputIndex] += linearConfig->weight->dataTensor->data[weightIndex]
                    *
                    lossQTensor->data[
                        lossIndex];*/

                /*printf("WeightGrad: %f\n", weightGrad);
                printf("PropLoss: %f\n", propLoss);*/

            }

            uint8_t *biasGradAddress = &linearConfig->bias->gradTensor->data[biasByteIndex];
            float32_t biasGrad = readBytesAsFloat32(biasGradAddress);
            biasGrad += loss;
            writeFloat32ToByteArray(biasGrad, biasGradAddress);
            //printf("BiasGrad: %f\n", biasGrad);

            /*linearConfig->bias->gradTensor->data[lossIndex] += lossQTensor->data[lossIndex];*/
        }

        break;
    case FLOAT64_Q:
        break;
    default:
        break;
    }

    qTensor_t *propLossQTensor = initQTensor(propagatedLoss, numberOfDimensions, dimensions, outputQuantization);

    free(propagatedLoss);
    free(dimensions);

    return propLossQTensor;
}

linearConfig_t *initLinearConfigWithWeightBias(parameterQTensor_t *weightQTensor,
                                               parameterQTensor_t *biasQTensor) {
    linearConfig_t *config = calloc(1, sizeof(linearConfig_t));

    config->weight = weightQTensor;
    config->bias = biasQTensor;

    return config;
}

linearConfig_t *initLinearConfigWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}

layerForward_t *initLinearLayerForwardWithWeightBias(parameterQTensor_t *weightQTensor,
                                                     parameterQTensor_t *biasQTensor) {

    if (weightQTensor->dataTensor->numberOfDimensions != 2) {
        printf("Error: linear layer weights must be 2D (outputsize, inputsize)\n");
        return NULL;
    }

    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->config = initLinearConfigWithWeightBias(weightQTensor, biasQTensor);
    layerForward->type = LINEAR;

    return layerForward;
}

layerForward_t *initLinearLayerWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}

layerForwardBackward_t *initLinearLayerForwardBackwardWithWeightBias(
    parameterQTensor_t *weightQTensor, parameterQTensor_t *biasQTensor) {

    if (weightQTensor->dataTensor->numberOfDimensions != 2) {
        printf("Error: linear layer weights must be 2D (outputsize, inputsize)\n");
        return NULL;
    }

    layerForwardBackward_t *layerForwardBackward = calloc(1, sizeof(layerForwardBackward_t));
    layerForwardBackward->config =
        initLinearConfigWithWeightBias(weightQTensor, biasQTensor);
    layerForwardBackward->type = LINEAR;

    return layerForwardBackward;
}

layerForwardBackward_t *initLinearLayerBackwardWithInputOutputSize(size_t inputSize,
    size_t outputSize) {
    ;
}
