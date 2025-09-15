#ifndef CROSSENTROPY_H
#define CROSSENTROPY_H
#include <stddef.h>

float crossEntropyForward(float *logits, int target, size_t inputSize);

float* crossEntropyBackward(float *logits, int target, size_t inputSize);

#endif //CROSSENTROPY_H
