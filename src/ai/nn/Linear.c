#define SOURCE_FILE "AI_NN_LINEAR"

#include "Linear.h"

#include <Common.h>
#include <string.h>

tensor_t *initLinearOutputTensor(linearConfig_t *linearConfig, tensor_t *inputTensor) {
    size_t totalOutputSize = linearConfig->weight->tensor->dimensions[0];
    tensor_t *outputTensor = calloc(1, sizeof(tensor_t));

    outputTensor->numberOfDimensions = 2;
    outputTensor->dimensions = calloc(outputTensor->numberOfDimensions, sizeof(size_t));
    // Needs to change for batch processing
    outputTensor->dimensions[0] = 1;
    outputTensor->dimensions[1] = totalOutputSize;

    outputTensor->data = calloc(totalOutputSize, sizeof(float));
    return outputTensor;
}

tensor_t *linearForward(void *config, tensor_t *inputTensor) {
    linearConfig_t *linearConfig = config;
    size_t weightIndex = 0;

    tensor_t *outputTensor = initLinearOutputTensor(linearConfig, inputTensor);
    size_t inputSize = 0;
    size_t outputSize = 0;

    outputSize = linearConfig->weight->tensor->dimensions[0];
    inputSize = linearConfig->weight->tensor->dimensions[1];


    for (size_t i = 0; i < outputSize; i++) {
        float result = 0;
        for (size_t j = 0; j < inputSize; j++) {
            weightIndex = i * inputSize + j;
            result += inputTensor->data[j] * linearConfig->weight->tensor->data[weightIndex];
        }
        outputTensor->data[i] = result + linearConfig->bias->tensor->data[i];
    }

    return outputTensor;
}

tensor_t *linearBackward(void *config, tensor_t *gradTensor, tensor_t *inputTensor) {
    linearConfig_t *linearConfig = config;

    size_t outputSize = linearConfig->weight->tensor->dimensions[0];
    size_t inputSize = linearConfig->weight->tensor->dimensions[1];

    tensor_t *propagatedLoss = calloc(1, sizeof(tensor_t));
    size_t totalInputSize = calcTotalNumberOfElementsByTensor(inputTensor);
    propagatedLoss->data = calloc(totalInputSize, sizeof(float));

    propagatedLoss->numberOfDimensions = inputTensor->numberOfDimensions;
    propagatedLoss->dimensions = calloc(propagatedLoss->numberOfDimensions, sizeof(size_t));
    memcpy(propagatedLoss->dimensions, inputTensor->dimensions,
           propagatedLoss->numberOfDimensions * sizeof(size_t));

    size_t weightIndex = 0;
    for (size_t lossIndex = 0; lossIndex < outputSize; lossIndex++) {
        for (size_t inputIndex = 0; inputIndex < inputSize; inputIndex++) {
            weightIndex = lossIndex * inputSize + inputIndex;

            linearConfig->weight->grad[weightIndex] += gradTensor->data[lossIndex] * inputTensor->
                data[inputIndex];
            propagatedLoss->data[inputIndex] += linearConfig->weight->tensor->data[weightIndex] *
                gradTensor->data[
                    lossIndex];
        }
        linearConfig->bias->grad[lossIndex] += gradTensor->data[lossIndex];
    }

    return propagatedLoss;
}

linearConfig_t *initLinearConfigWithWeightBias(parameterTensor_t *weightTensor,
                                               parameterTensor_t *biasTensor) {
    linearConfig_t *config = calloc(1, sizeof(linearConfig_t));

    config->weight = weightTensor;
    config->bias = biasTensor;

    return config;
}

linearConfig_t *initLinearConfigWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}

layerForward_t *initLinearLayerForwardWithWeightBias(parameterTensor_t *weightTensor,
                                                     parameterTensor_t *biasTensor) {

    if (weightTensor->tensor->numberOfDimensions != 2) {
        printf("Error: linear layer weights must be 2D (outputsize, inputsize)\n");
        return NULL;
    }

    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->config = initLinearConfigWithWeightBias(weightTensor, biasTensor);
    layerForward->type = LINEAR;

    return layerForward;
}

layerForward_t *initLinearLayerWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}

layerForwardBackward_t *initLinearLayerForwardBackwardWithWeightBias(
    parameterTensor_t *weightTensor, parameterTensor_t *biasTensor) {

    if (weightTensor->tensor->numberOfDimensions != 2) {
        printf("Error: linear layer weights must be 2D (outputsize, inputsize)\n");
        return NULL;
    }

    layerForwardBackward_t *layerForwardBackward = calloc(1, sizeof(layerForwardBackward_t));
    layerForwardBackward->config =
        initLinearConfigWithWeightBias(weightTensor, biasTensor);
    layerForwardBackward->type = LINEAR;

    return layerForwardBackward;
}

layerForwardBackward_t *initLinearLayerBackwardWithInputOutputSize(size_t inputSize,
    size_t outputSize) {
    ;
}
