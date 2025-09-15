#ifndef CROSSENTROPY_H
#define CROSSENTROPY_H

#include "AiHelpers.h"


float crossEntropyForwardWithIndex(float *softmaxOutput, int classIndex);

float crossEntropyForwardWithDistribution(float *softmaxOutput, float *distribution, size_t inputSize);

float *crossEntropyBackwardWithIndex(float *softmaxOutput, int classIndex, size_t inputSize);

float *crossEntropyBackwardWithDistribution(float *softmaxOutput, float *distribution, size_t inputSize);

#endif //CROSSENTROPY_H
