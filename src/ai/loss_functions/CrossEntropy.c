#include "CrossEntropy.h"
#include "Softmax.h"
#include "math.h"
#include "AiHelpers.h"


float crossEntropyForward(float *logits, int target, size_t inputSize) {
    softmaxConfig_t *config = initSoftmaxConfig(inputSize);
    float *probabilities = softmaxForward(config, logits, inputSize);
    float loss = -logf(probabilities[target]);

    return loss;
}

float* crossEntropyBackward(float *logits, int target, size_t inputSize) {
    softmaxConfig_t *config = initSoftmaxConfig(inputSize);
    float *probabilities = softmaxForward(config, logits, inputSize);

    float t[inputSize];
    t[target]=1.f;

    float *result = calloc(inputSize, sizeof(float));

    for (size_t i = 0; i < inputSize; i++) {
        result[i] = probabilities[i] - t[i];
    }

    return result;
}



