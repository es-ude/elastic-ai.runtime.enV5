#define SOURCE_FILE "AI_NN_LINEAR"

#include "Linear.h"

#include <Common.h>
#include <stdlib.h>
#include <string.h>

qTensor_t *initLinearOutputTensor(linearConfig_t *linearConfig, qTensor_t *inputQTensor) {
    size_t totalOutputSize = linearConfig->weight->dataTensor->dimensions[0];
    qTensor_t *outputQTensor = calloc(1, sizeof(qTensor_t));

    outputQTensor->numberOfDimensions = 2;
    outputQTensor->dimensions = calloc(outputQTensor->numberOfDimensions, sizeof(size_t));
    // Needs to change for batch processing
    outputQTensor->dimensions[0] = 1;
    outputQTensor->dimensions[1] = totalOutputSize;

    outputQTensor->data = calloc(totalOutputSize, sizeof(float));
    return outputQTensor;
}

qTensor_t *linearForward(void *config, qTensor_t *inputQTensor) {
    linearConfig_t *linearConfig = config;
    size_t weightIndex = 0;

    qTensor_t *outputTensor = initLinearOutputTensor(linearConfig, inputQTensor);
    size_t inputSize = 0;
    size_t outputSize = 0;

    outputSize = linearConfig->weight->dataTensor->dimensions[0];
    inputSize = linearConfig->weight->dataTensor->dimensions[1];


    for (size_t i = 0; i < outputSize; i++) {
        float result = 0;
        for (size_t j = 0; j < inputSize; j++) {
            weightIndex = i * inputSize + j;
            result += inputQTensor->data[j] * linearConfig->weight->dataTensor->data[weightIndex];
        }
        outputTensor->data[i] = result + linearConfig->bias->dataTensor->data[i];
    }

    return outputTensor;
}

qTensor_t *linearBackward(void *config, qTensor_t *gradQTensor, qTensor_t *inputQTensor) {
    linearConfig_t *linearConfig = config;

    size_t outputSize = linearConfig->weight->dataTensor->dimensions[0];
    size_t inputSize = linearConfig->weight->dataTensor->dimensions[1];

    qTensor_t *propagatedLoss = calloc(1, sizeof(qTensor_t));
    size_t totalInputSize = calcTotalNumberOfElementsByTensor(inputQTensor);
    propagatedLoss->data = calloc(totalInputSize, sizeof(float));

    propagatedLoss->numberOfDimensions = inputQTensor->numberOfDimensions;
    propagatedLoss->dimensions = calloc(propagatedLoss->numberOfDimensions, sizeof(size_t));
    memcpy(propagatedLoss->dimensions, inputQTensor->dimensions,
           propagatedLoss->numberOfDimensions * sizeof(size_t));

    size_t weightIndex = 0;
    for (size_t lossIndex = 0; lossIndex < outputSize; lossIndex++) {
        for (size_t inputIndex = 0; inputIndex < inputSize; inputIndex++) {
            weightIndex = lossIndex * inputSize + inputIndex;

            linearConfig->weight->gradTensor->data[weightIndex] += gradQTensor->data[lossIndex] * inputQTensor->
                data[inputIndex];
            propagatedLoss->data[inputIndex] += linearConfig->weight->dataTensor->data[weightIndex] *
                gradQTensor->data[
                    lossIndex];
        }
        linearConfig->bias->gradTensor->data[lossIndex] += gradQTensor->data[lossIndex];
    }

    return propagatedLoss;
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
