#include "CrossEntropy.h"
#include "Softmax.h"
#include "math.h"
#include "AiHelpers.h"

#include <stdio.h>
#include <string.h>


float crossEntropyForwardWithIndex(float *softmaxOutput, int classIndex) {
    float loss = -logf(softmaxOutput[classIndex]);

    return loss;
}


float crossEntropyForwardWithDistribution(float *softmaxOutput, float *distribution, size_t inputSize) {
    float loss = 0.f;
    for (size_t i = 0; i < inputSize; i++) {
        loss += distribution[i] * -logf(softmaxOutput[i]);
    }

    return loss;
}


float *crossEntropyBackwardWithIndex(float *softmaxOutput, int classIndex, size_t inputSize) {
    float t[inputSize];
    memset(t, 0, sizeof(t));

    t[classIndex] = 1.f;

    float *result = calloc(inputSize, sizeof(float));

    for (size_t i = 0; i < inputSize; i++) {
        result[i] = softmaxOutput[i] - t[i];
    }

    return result;
}

float *crossEntropyBackwardWithDistribution(float *softmaxOutput, float *distribution, size_t inputSize) {
    float *result = calloc(inputSize, sizeof(float));

    for (size_t i = 0; i < inputSize; i++) {
        result[i] = softmaxOutput[i] - distribution[i];
    }

    return result;
}
