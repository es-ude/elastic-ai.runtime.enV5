#ifndef ENV5_RUNTIME_ADD_H
#define ENV5_RUNTIME_ADD_H
#include "Tensor.h"

void addFloatTensors(tensor_t* aTensor, tensor_t* bTensor, tensor_t* outputTensor);
void addInt32Tensors(tensor_t* aTensor, tensor_t* bTensor, tensor_t* outputTensor);

#endif // ENV5_RUNTIME_ADD_H
