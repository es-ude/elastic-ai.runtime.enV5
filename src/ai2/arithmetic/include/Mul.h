#ifndef ENV5_RUNTIME_MUL_H
#define ENV5_RUNTIME_MUL_H

#include "Tensor.h"

int32_t mulInt32s(int32_t a, int32_t b);

float mulFloats(float a, float b);

void mulInt32Tensors(tensor_t* a, tensor_t* b, tensor_t* outputTensor);
void mulInt32TensorsInplace(tensor_t* a, tensor_t* b);

void mulInt32ElementWithTensor(tensor_t* a, int32_t b, tensor_t* outputTensor);
void mulInt32ElementWithTensorInplace(tensor_t* a, int32_t b);

void mulFloatTensors(tensor_t* a, tensor_t* b, tensor_t* outputTensor);
void mulFloatTensorsInplace(tensor_t* a, tensor_t* b);

void mulFloatElementWithTensor(tensor_t* a, float b, tensor_t* outputTensor);
void mulFloatElementWithTensorInplace(tensor_t* a, float b);

size_t getMulInstructionCounter();

#endif // ENV5_RUNTIME_MUL_H
