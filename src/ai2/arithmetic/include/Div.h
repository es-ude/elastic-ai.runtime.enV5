#ifndef ENV5_RUNTIME_DIV_H
#define ENV5_RUNTIME_DIV_H
#include <stdbool.h>

#include "Tensor.h"

int32_t divInt32s(int32_t a, int32_t b);

float divFloat32s(float a, float b);

void divInt32Tensors(tensor_t* a, tensor_t* b, tensor_t* outputTensor);
void divInt32TensorsInplace(tensor_t* a, tensor_t* b);

void divInt32ElementWithInt32Tensor(tensor_t* a, int32_t b, tensor_t* outputTensor);
void divInt32ElementWithInt32TensorInplace(tensor_t* a, int32_t b);

void divFloat32Tensors(tensor_t* a, tensor_t* b, tensor_t* outputTensor);
void divFloat32TensorsInplace(tensor_t* a, tensor_t* b);

void divFloat32ElementWithFloat32Tensor(tensor_t* a, float b, tensor_t* outputTensor);
void divFloat32ElementWithFloat32TensorInplace(tensor_t* a, float b);

void divSymInt32Tensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor);

void divSymInt32TensorsInplace(tensor_t *aTensor, tensor_t *bTensor);

size_t getDivInstructionCounter();

#endif // ENV5_RUNTIME_DIV_H
