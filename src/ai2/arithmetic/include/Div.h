#ifndef ENV5_RUNTIME_DIV_H
#define ENV5_RUNTIME_DIV_H
#include "Tensor.h"

int32_t divInt32s(int32_t a, int32_t b);

float divFloats(float a, float b);

void divInt32Tensors(tensor_t* a, tensor_t* b, tensor_t* outputTensor);
void divInt32TensorsInplace(tensor_t* a, tensor_t* b);

void divInt32ElementWithTensor(tensor_t* a, int32_t b, tensor_t* outputTensor);
void divInt32ElementWithTensorInplace(tensor_t* a, int32_t b);

void divFloatTensors(tensor_t* a, tensor_t* b, tensor_t* outputTensor);
void divFloatTensorsInplace(tensor_t* a, tensor_t* b);

void divFloatElementWithTensor(tensor_t* a, float b, tensor_t* outputTensor);
void divFloatElementWithTensorInplace(tensor_t* a, float b);

size_t getDivInstructionCounter();

#endif // ENV5_RUNTIME_DIV_H
