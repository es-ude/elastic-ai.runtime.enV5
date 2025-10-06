#ifndef CROSSENTROPY_H
#define CROSSENTROPY_H

#include "AiHelpers.h"

float crossEntropyForward(const float *softmaxOutput, const float *distribution, const size_t inputSize);

float *crossEntropySoftmaxBackward(const float *softmaxOutput, const float *distribution, const size_t inputSize);

#endif //CROSSENTROPY_H
