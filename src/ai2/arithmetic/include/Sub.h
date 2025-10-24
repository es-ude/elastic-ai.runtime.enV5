#ifndef ENV5_RUNTIME_SUB_H
#define ENV5_RUNTIME_SUB_H
#include "Tensor.h"

int32_t subInt32s(int32_t a, int32_t b);

float subFloats(float a, float b);

size_t getSubInstructionCounter();

void subInt32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor);
void subInt32TensorsInplace(tensor_t *a, tensor_t *b);

void subInt32ElementWithTensor(tensor_t *a, int32_t b, tensor_t *outputTensor);
void subInt32ElementWithTensorInplace(tensor_t *a, int32_t b);

void subFloatTensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor);
void subFloatTensorsInplace(tensor_t *a, tensor_t *b);

void subFloatElementWithTensor(tensor_t *a, float b, tensor_t *outputTensor);
void subFloatElementWithTensorInplace(tensor_t *a, float b);
#endif // ENV5_RUNTIME_SUB_H
