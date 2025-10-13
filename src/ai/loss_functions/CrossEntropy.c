#include "CrossEntropy.h"
#include "Softmax.h"
#include "math.h"
#include "AiHelpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float crossEntropyForward(const float *softmaxOutput, const float *distribution, const size_t inputSize) {
    float loss = 0.f;
    for (size_t i = 0; i < inputSize; i++) {
        loss += distribution[i] * -logf(softmaxOutput[i]);
    }

    return loss;
}

qTensor_t *crossEntropySoftmaxBackward(qTensor_t *softmaxOutput, qTensor_t *distribution) {

    size_t totalInputSize = calcTotalNumberOfElementsByTensor(softmaxOutput);

    qTensor_t *lossTensor = calloc(1, sizeof(qTensor_t));
    lossTensor->numberOfDimensions = softmaxOutput->numberOfDimensions;
    lossTensor->dimensions = calloc(lossTensor->numberOfDimensions, sizeof(size_t));
    memcpy(lossTensor->dimensions, softmaxOutput->dimensions, lossTensor->numberOfDimensions * sizeof(size_t));
    lossTensor->data = calloc(totalInputSize, sizeof(float));

    for (size_t i = 0; i < totalInputSize; i++) {
        lossTensor->data[i] = softmaxOutput->data[i] - distribution->data[i];
    }

    return lossTensor;
}
