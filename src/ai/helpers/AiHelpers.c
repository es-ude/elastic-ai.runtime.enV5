#define SOURCE_FILE "AI-HELPERS"

#include "AiHelpers.h"
#include "Conv1d.h"
#include "Linear.h"
#include "Softmax.h"
#include "ReLU.h"
#include "CrossEntropy.h"
#include "MSE.h"

#include <string.h>

const layerFunctionEntry_t layerFunctions[] = {
    [LINEAR] = {linearForward, linearBackward},
    [RELU] = {ReLUForward, ReLUBackward},
    [CONV1D] = {conv1dForward, conv1dBackward},
    [SOFTMAX] = {softmaxForward, softmaxBackward},
    [FLATTEN] = {flattenForward, NULL}
};

const lossFunctionEntry_t lossFuntions[] = {
    [MSE] = {MSELossBackward},
    [CROSS_ENTROPY] = {crossEntropySoftmaxBackward}
};


size_t calcTensorDataSize(size_t numberOfDimensions, size_t *dimensions) {
    size_t dataSize = 1;
    for (size_t i = 0; i < numberOfDimensions; i++) {
        dataSize *= dimensions[i];
    }
    return dataSize;
}

size_t calcTotalNumberOfElementsByTensor(tensor_t *tensor) {
    size_t totalSize = 1;
    for (size_t i = 0; i < tensor->numberOfDimensions; i++) {
        totalSize *= tensor->dimensions[i];
    }
    return totalSize;
}

tensor_t *initTensor(float *data, size_t numberOfDimensions, size_t *dimensions) {
    tensor_t *tensor = calloc(1, sizeof(tensor_t));

    size_t dataSize = calcTensorDataSize(numberOfDimensions, dimensions);

    tensor->data = calloc(dataSize, sizeof(float));
    memcpy(tensor->data, data, dataSize * sizeof(float));

    tensor->numberOfDimensions = numberOfDimensions;

    tensor->dimensions = calloc(numberOfDimensions, sizeof(size_t));
    memcpy(tensor->dimensions, dimensions, numberOfDimensions * sizeof(size_t));

    return tensor;
}

parameterTensor_t *initParameterTensor(float *data, size_t numberOfDimensions, size_t *dimensions) {
    parameterTensor_t *parameterTensor = calloc(1, sizeof(parameterTensor_t));

    parameterTensor->tensor = initTensor(data, numberOfDimensions, dimensions);
    size_t dataSize = calcTensorDataSize(numberOfDimensions, dimensions);

    parameterTensor->grad = calloc(dataSize, sizeof(float));

    return parameterTensor;
}



tensor_t *initInputTensorForNextLayer(layerForward_t *currentLayer,
                                         tensor_t *currentInputTensor) {
    tensor_t *inputTensorNextLayer = calloc(1, sizeof(tensor_t));
    switch (currentLayer->type) {
    case LINEAR:
        inputTensorNextLayer = initLinearOutputTensor(currentLayer->config, currentInputTensor);
        break;
    case RELU:
        inputTensorNextLayer->numberOfDimensions = currentInputTensor->numberOfDimensions;
        inputTensorNextLayer->dimensions = calloc(inputTensorNextLayer->numberOfDimensions,
                                                  sizeof(size_t));
        memcpy(inputTensorNextLayer->dimensions, currentInputTensor->dimensions,
               inputTensorNextLayer->numberOfDimensions * sizeof(size_t));
        break;
    case CONV1D:
        inputTensorNextLayer = initConv1dOutputTensor(currentLayer->config, currentInputTensor);
        break;
    case SOFTMAX:
        inputTensorNextLayer->numberOfDimensions = currentInputTensor->numberOfDimensions;
        inputTensorNextLayer->dimensions = calloc(inputTensorNextLayer->numberOfDimensions,
                                                  sizeof(size_t));
        memcpy(inputTensorNextLayer->dimensions, currentInputTensor->dimensions,
               inputTensorNextLayer->numberOfDimensions * sizeof(size_t));
        break;
    default:
        printf("SWITCH CASE NOT FOUND\n");
        break;
    }
    return inputTensorNextLayer;
}

tensor_t *sequentialForward(layerForward_t **network, size_t sizeNetwork, tensor_t *inputTensor) {
    for (size_t i = 0; i < sizeNetwork; i++) {


        layerType_t type = network[i]->type;
        forward *fwd = layerFunctions[type].forwardFunc;
        inputTensor = fwd(network[i]->config, inputTensor);

    }
    return inputTensor;
}

loss *getLossFunctionByType(lossFunctionType_t lossType) {
    loss *lossFunction = calloc(1, sizeof(loss));
    switch (lossType) {
    case MSE:
        lossFunction = MSELossBackward;
        break;
    case CROSS_ENTROPY:
        lossFunction = crossEntropySoftmaxBackward;
        break;
    default:
        printf("Loss type not found");
        break;
    }
    return lossFunction;
}


flattenConfig_t *initFlattenConfig(size_t size) {
    flattenConfig_t *config = calloc(1, sizeof(flattenConfig_t));
    config->size = size;
    return config;
}

layerForward_t *initFlattenLayerForward(tensor_t *inputTensor) {
    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->type = FLATTEN;
    size_t totalInputSize = calcTotalNumberOfElementsByTensor(inputTensor);
    layerForward->config = initReLUConfig(totalInputSize);
    return layerForward;
}

tensor_t *flattenForward(void *config, tensor_t *inputTensor) {
    tensor_t *outputTensor = calloc(1, sizeof(tensor_t));

    // Calculate total size
    size_t totalSize = 1;
    for (size_t i = 0; i < inputTensor->numberOfDimensions; i++) {
        totalSize *= inputTensor->dimensions[i];
    }

    outputTensor->numberOfDimensions = 1;
    outputTensor->dimensions = calloc(1, sizeof(size_t));
    outputTensor->dimensions[0] = totalSize;

    // Copy data (or just reuse pointer if you manage memory differently)
    outputTensor->data = calloc(totalSize, sizeof(float));
    memcpy(outputTensor->data, inputTensor->data, totalSize * sizeof(float));

    return outputTensor;
}

/*! IMPORTANT: We assume, that if you use Cross Entropy as your loss function,
 * you also use Softmax with it. We introduce Softmax as a dedicated Layer,
 * but in the backward pass it is ignored. We do this, because the Cross Entropy Backward
 * already takes the Softmax Backward into account.
 */
trainingStats_t *sequentialCalculateGrads(layerForwardBackward_t **network,
                                          size_t sizeNetwork,
                                          lossFunctionType_t lossFunctionType,
                                          tensor_t *inputTensor,
                                          tensor_t *labelTensor) {



    tensor_t **layerOutputs = calloc((sizeNetwork + 1), sizeof(tensor_t *));
    layerOutputs[0] = inputTensor;


    // Forward Pass
    for (size_t i = 0; i < sizeNetwork; i++) {
        layerForwardBackward_t *currentLayer = network[i];

        forward *fwd = layerFunctions[currentLayer->type].forwardFunc;
        layerOutputs[i + 1] = fwd(currentLayer->config, layerOutputs[i]);
    }

    // Loss
    loss *lossFunction = getLossFunctionByType(lossFunctionType);
    tensor_t *grad = lossFunction(layerOutputs[sizeNetwork], labelTensor);
    trainingStats_t *trainingStats = calloc(1, sizeof(trainingStats_t));
    trainingStats->loss = grad->data;

    size_t backwardIndex = sizeNetwork - 1;
    if (lossFunctionType == CROSS_ENTROPY) {
        backwardIndex -= 1;
    }
    // Backward Pass
    for (int i = (int)(backwardIndex); i >= 0; i--) {
        backward *bwd = layerFunctions[network[i]->type].backwardFunc;
        grad = bwd(network[i]->config, grad, layerOutputs[i]);
    }

    trainingStats->output = grad->data;

    return trainingStats;
}
