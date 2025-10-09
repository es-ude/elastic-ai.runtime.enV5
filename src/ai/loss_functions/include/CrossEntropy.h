#ifndef CROSSENTROPY_H
#define CROSSENTROPY_H

#include "AiHelpers.h"

float crossEntropyForward(const float *softmaxOutput, const float *distribution, const size_t inputSize);

tensor_t *crossEntropySoftmaxBackward(tensor_t *softmaxOutput, tensor_t *distribution);

#endif //CROSSENTROPY_H
