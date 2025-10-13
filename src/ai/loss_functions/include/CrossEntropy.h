#ifndef CROSSENTROPY_H
#define CROSSENTROPY_H

#include "AiHelpers.h"

float crossEntropyForward(const float *softmaxOutput, const float *distribution, const size_t inputSize);

qTensor_t *crossEntropySoftmaxBackward(qTensor_t *softmaxOutput, qTensor_t *distribution);

#endif //CROSSENTROPY_H
