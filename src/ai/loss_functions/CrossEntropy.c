#include "CrossEntropy.h"
#include "Softmax.h"
#include "math.h"
#include "AiHelpers.h"

#include <stdio.h>
#include <string.h>

float crossEntropyForward(const float *softmaxOutput, const float *distribution, const size_t inputSize) {
    float loss = 0.f;
    for (size_t i = 0; i < inputSize; i++) {
        loss += distribution[i] * -logf(softmaxOutput[i]);
    }

    return loss;
}

tensor_t *crossEntropySoftmaxBackward(tensor_t *softmaxOutput, tensor_t *distribution) {

    size_t totalInputSize = calcTotalNumberOfElementsByTensor(softmaxOutput);

    tensor_t *lossTensor = calloc(1, sizeof(tensor_t));
    lossTensor->numberOfDimensions = softmaxOutput->numberOfDimensions;
    lossTensor->dimensions = calloc(lossTensor->numberOfDimensions, sizeof(size_t));
    memcpy(lossTensor->dimensions, softmaxOutput->dimensions, lossTensor->numberOfDimensions * sizeof(size_t));
    lossTensor->data = calloc(totalInputSize, sizeof(float));

    for (size_t i = 0; i < totalInputSize; i++) {
        lossTensor->data[i] = softmaxOutput->data[i] - distribution->data[i];
    }

    return lossTensor;
}
