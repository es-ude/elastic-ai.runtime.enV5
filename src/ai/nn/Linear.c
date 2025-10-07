#define SOURCE_FILE "AI_NN_LINEAR"

#include "Linear.h"

#include <Common.h>
#include <string.h>

tensor_t *initOutputTensor(linearConfig_t *linearConfig, tensor_t *inputTensor) {
    size_t outputSize = linearConfig->weight->tensor->dimensions[0];
    tensor_t *outputTensor = malloc(sizeof(size_t));
    if (inputTensor->numberOfDimensions == 1) {
        outputTensor->numberOfDimensions = 1;
        outputTensor->dimensions = malloc(sizeof(size_t));
        outputTensor->dimensions[0] = outputSize;
        outputTensor->data = calloc(outputSize, sizeof(float));
    }
    if (inputTensor->numberOfDimensions == 2) {
        outputTensor->numberOfDimensions = 2;
        outputTensor->dimensions = calloc(outputTensor->numberOfDimensions, sizeof(size_t));
        outputTensor->dimensions[0] = inputTensor->dimensions[0];
        outputTensor->dimensions[1] = outputSize;
        size_t totalOutputSize = calcTensorDataSize(outputTensor->numberOfDimensions,
                                                    outputTensor->dimensions);
        outputTensor->data = calloc(totalOutputSize, sizeof(float));
    }
    return outputTensor;
}

tensor_t *linearForward(void *config, tensor_t *inputTensor) {
    linearConfig_t *linearConfig = config;
    size_t weightIndex = 0;

    tensor_t *outputTensor = initOutputTensor(linearConfig, inputTensor);
    size_t inputSize = 0;
    size_t outputSize = 0;

    if (linearConfig->weight->tensor->numberOfDimensions == 2) {
        outputSize = linearConfig->weight->tensor->dimensions[0];
        inputSize = linearConfig->weight->tensor->dimensions[1];
    }
    else if (linearConfig->weight->tensor->numberOfDimensions == 1) {
        outputSize = linearConfig->bias->tensor->
                     dimensions[0];
        inputSize = linearConfig->weight->tensor->dimensions[0] / linearConfig->bias->tensor->dimensions[0];
    }
    else {
        printf("Weight dimensions not supported");
        return NULL;
    }


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
    size_t inputSize = 0;
    size_t outputSize = 0;

    if (linearConfig->weight->tensor->numberOfDimensions == 2) {
        outputSize = linearConfig->weight->tensor->dimensions[0];
        inputSize = linearConfig->weight->tensor->dimensions[1];
    }
    else if (linearConfig->weight->tensor->numberOfDimensions == 1) {
        outputSize = linearConfig->bias->tensor->
                     dimensions[0];
        inputSize = linearConfig->weight->tensor->dimensions[0] / linearConfig->bias->tensor->dimensions[0];
    }
    else {
        printf("Weight dimensions not supported");
        return NULL;
    }

    tensor_t *propagatedLoss = malloc(sizeof(tensor_t));
    size_t totalInputSize = calcTotalNumberOfElementsByTensor(inputTensor);
    propagatedLoss->data = calloc(totalInputSize, sizeof(float));

    propagatedLoss->numberOfDimensions = inputTensor->numberOfDimensions;

    propagatedLoss->dimensions = calloc(propagatedLoss->numberOfDimensions, sizeof(size_t));
    memcpy(propagatedLoss->dimensions, inputTensor->dimensions, propagatedLoss->numberOfDimensions * sizeof(size_t));

    size_t weightIndex = 0;
    for (size_t lossIndex = 0; lossIndex < outputSize; lossIndex++) {
        for (size_t inputIndex = 0; inputIndex < inputSize; inputIndex++) {
            weightIndex = lossIndex * inputSize + inputIndex;

            linearConfig->weight->grad[weightIndex] += gradTensor->data[lossIndex] * inputTensor->
                data[inputIndex];
            propagatedLoss->data[inputIndex] += linearConfig->weight->tensor->data[weightIndex] * gradTensor->data[
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
    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->config = initLinearConfigWithWeightBias(weightTensor, biasTensor);
    layerForward->type = LINEAR;

    return layerForward;
}

layerForward_t *initLinearLayerWithInputOutputSize(size_t inputSize, size_t outputSize) {
    ;
}

layerForwardBackward_t *initLinearLayerForwardBackwardWithWeightBias(
    tensor_t *weightTensor, tensor_t *biasTensor) {
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
