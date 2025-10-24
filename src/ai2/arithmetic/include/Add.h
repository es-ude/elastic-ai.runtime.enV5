#ifndef ENV5_RUNTIME_ADD_H
#define ENV5_RUNTIME_ADD_H
#include "Tensor.h"

float addFloats(float a, float b);

int32_t addInt32s(int32_t a, int32_t b);

void addInt32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor);
void addInt32TensorsInplace(tensor_t *a, tensor_t *b);

void addInt32ElementWithTensor(tensor_t *a, int32_t b, tensor_t *outputTensor);
void addInt32ElementWithTensorInplace(tensor_t *a, int32_t b);

void addFloatTensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor);
void addFloatTensorsInplace(tensor_t *a, tensor_t *b);

void addFloatElementWithTensor(tensor_t *a, float b, tensor_t *outputTensor);
void addFloatElementWithTensorInplace(tensor_t *a, float b);

void addTensorsInplace(tensor_t *a, tensor_t *b);

size_t getAddInstructionCounter();

#endif // ENV5_RUNTIME_ADD_H
