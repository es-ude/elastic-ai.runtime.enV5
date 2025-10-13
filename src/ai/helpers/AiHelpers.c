#define SOURCE_FILE "AI-HELPERS"

#include "AiHelpers.h"
#include "Conv1d.h"
#include "Linear.h"
#include "Softmax.h"
#include "ReLU.h"
#include "CrossEntropy.h"
#include "MSE.h"

#include <stdlib.h>
#include <string.h>

const layerFunctionEntry_t layerFunctions[] = {
    [LINEAR] = {linearForward, linearBackward},
    [RELU] = {ReLUForward, ReLUBackward},
    [CONV1D] = {conv1dForward, conv1dBackward},
    [SOFTMAX] = {softmaxForward, softmaxBackward},
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

size_t calcTotalNumberOfElementsByTensor(qTensor_t *qTensor) {
    size_t totalSize = 1;
    for (size_t i = 0; i < qTensor->numberOfDimensions; i++) {
        totalSize *= qTensor->dimensions[i];
    }
    return totalSize;
}






qTensor_t *sequentialForward(layerForward_t **network, size_t sizeNetwork, qTensor_t *inputQTensor) {
    for (size_t i = 0; i < sizeNetwork; i++) {

        layerType_t type = network[i]->type;
        forward *fwd = layerFunctions[type].forwardFunc;
        inputQTensor = fwd(network[i]->config, inputQTensor);

    }
    return inputQTensor;
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

/*! IMPORTANT: We assume, that if you use Cross Entropy as your loss function,
 * you also use Softmax with it. We introduce Softmax as a dedicated Layer,
 * but in the backward pass it is ignored. We do this, because the Cross Entropy Backward
 * already takes the Softmax Backward into account.
 */
trainingStats_t *sequentialCalculateGrads(layerForwardBackward_t **network,
                                          size_t sizeNetwork,
                                          lossFunctionType_t lossFunctionType,
                                          qTensor_t *inputQTensor,
                                          qTensor_t *labelQTensor) {

    qTensor_t **layerOutputs = calloc((sizeNetwork + 1), sizeof(qTensor_t *));
    layerOutputs[0] = inputQTensor;

    // Forward Pass
    for (size_t i = 0; i < sizeNetwork; i++) {
        layerForwardBackward_t *currentLayer = network[i];

        forward *fwd = layerFunctions[currentLayer->type].forwardFunc;
        layerOutputs[i + 1] = fwd(currentLayer->config, layerOutputs[i]);
    }

    // Loss
    loss *lossFunction = getLossFunctionByType(lossFunctionType);
    // TODO adapt loss functions
    qTensor_t *gradQTensor = lossFunction(layerOutputs[sizeNetwork], labelQTensor);
    trainingStats_t *trainingStats = calloc(1, sizeof(trainingStats_t));
    // TODO adapt trainingStats_t
    trainingStats->loss = gradQTensor->data;

    size_t backwardIndex = sizeNetwork - 1;
    if (lossFunctionType == CROSS_ENTROPY) {
        backwardIndex -= 1;
    }
    // Backward Pass
    for (int i = (int)(backwardIndex); i >= 0; i--) {
        backward *bwd = layerFunctions[network[i]->type].backwardFunc;
        gradQTensor = bwd(network[i]->config, gradQTensor, layerOutputs[i]);
    }


    trainingStats->output = gradQTensor->data;

    return trainingStats;
}
