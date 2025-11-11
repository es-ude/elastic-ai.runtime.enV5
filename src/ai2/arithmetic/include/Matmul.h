#ifndef ENV5_RUNTIME_MATMUL_H
#define ENV5_RUNTIME_MATMUL_H

#include <stdbool.h>

#include "Tensor.h"

typedef void(*matmulFunc_t)(tensor_t* aTensor, tensor_t* bTensor, tensor_t outputTensor);

void matmulInt32Tensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor);

void matmulFloat32Tensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor);

void matmulSymInt32Tensors(tensor_t* aTensor, tensor_t* bTensor, tensor_t* outputTensor);

size_t getMatmulInstructionCounter();

#endif // ENV5_RUNTIME_MATMUL_H
