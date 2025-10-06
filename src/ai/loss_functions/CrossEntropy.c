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

float *crossEntropySoftmaxBackward(const float *softmaxOutput, const float *distribution, const size_t inputSize) {
    float *result = calloc(inputSize, sizeof(float));

    for (size_t i = 0; i < inputSize; i++) {
        result[i] = softmaxOutput[i] - distribution[i];
    }

    return result;
}
