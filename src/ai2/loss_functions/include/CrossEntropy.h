#ifndef CROSSENTROPY_H
#define CROSSENTROPY_H
#include "Tensor.h"

float crossEntropyForwardFloat(tensor_t *softmaxOutput, tensor_t *distribution);

void crossEntropySoftmaxBackward(tensor_t *softmaxOutput, tensor_t *distribution, tensor_t *loss);

#endif //CROSSENTROPY_H
