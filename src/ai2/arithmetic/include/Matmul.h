#ifndef ENV5_RUNTIME_MATMUL_H
#define ENV5_RUNTIME_MATMUL_H

#include "Tensor.h"

void matmulInt32Tensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor);

void matmulFloatTensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor);

void matMulTensors(tensor_t *a, tensor_t *b, tensor_t *output);

size_t getMatmulInstructionCounter();


#endif // ENV5_RUNTIME_MATMUL_H
