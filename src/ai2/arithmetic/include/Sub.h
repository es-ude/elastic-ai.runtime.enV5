#ifndef ENV5_RUNTIME_SUB_H
#define ENV5_RUNTIME_SUB_H
#include "Tensor.h"

int32_t subInt32s(int32_t a, int32_t b);

float subFloat32s(float a, float b);

void subInt32Tensors(tensor_t* a, tensor_t* b, tensor_t* outputTensor);
void subInt32TensorsInplace(tensor_t* a, tensor_t* b);

void subInt32ElementWithInt32Tensor(tensor_t* a, int32_t b, tensor_t* outputTensor);
void subInt32ElementWithInt32TensorInplace(tensor_t* a, int32_t b);

void subFloat32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor);
void subFloat32TensorsInplace(tensor_t *a, tensor_t *b);

void subFloat32ElementWithFloat32Tensor(tensor_t* a, float b, tensor_t* outputTensor);
void subFloat32ElementWithFloat32TensorInplace(tensor_t* a, float b);

void subSymInt32Tensors(tensor_t* aTensor, tensor_t* bTensor, tensor_t* outputTensor);
void subSymInt32TensorsInplace(tensor_t* aTensor, tensor_t* bTensor);

size_t getSubInstructionCounter();
#endif // ENV5_RUNTIME_SUB_H
