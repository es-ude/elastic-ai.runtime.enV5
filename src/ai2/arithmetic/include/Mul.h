#ifndef ENV5_RUNTIME_MUL_H
#define ENV5_RUNTIME_MUL_H

#include <stdbool.h>

#include "Tensor.h"

int32_t mulInt32s(int32_t a, int32_t b);

float mulFloat32s(float a, float b);

void mulInt32Tensors(tensor_t* a, tensor_t* b, tensor_t* outputTensor);
void mulInt32TensorsInplace(tensor_t* a, tensor_t* b);

void mulInt32ElementWithInt32Tensor(tensor_t* a, int32_t b, tensor_t* outputTensor);
void mulInt32ElementWithInt32TensorInplace(tensor_t* a, int32_t b);

void mulFloat32Tensors(tensor_t* a, tensor_t* b, tensor_t* outputTensor);
void mulFloat32TensorsInplace(tensor_t* a, tensor_t* b);

void mulFloat32ElementWithFloat32Tensor(tensor_t* a, float b, tensor_t* outputTensor);
void mulFloat32ElementWithFloat32TensorInplace(tensor_t* a, float b);

void mulSymInt32Tensors(tensor_t* aTensor, tensor_t* bTensor, tensor_t* outputTensor);
void mulSymInt32TensorsInplace(tensor_t* aTensor, tensor_t* bTensor);

size_t getMulInstructionCounter();

#endif // ENV5_RUNTIME_MUL_H
