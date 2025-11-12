#ifndef ENV5_RUNTIME_ADD_H
#define ENV5_RUNTIME_ADD_H
#include <stdbool.h>

#include "Tensor.h"

float addFloat32s(float a, float b);
int32_t addInt32s(int32_t a, int32_t b);

void addInt32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor);
void addInt32TensorsInplace(tensor_t *a, tensor_t *b);

void addInt32ElementWithInt32Tensor(tensor_t *a, int32_t b, tensor_t *outputTensor);
void addInt32ElementWithInt32TensorInplace(tensor_t *a, int32_t b);

void addFloat32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor);
void addFloat32TensorsInplace(tensor_t *a, tensor_t *b);

void addFloat32ElementWithFloat32Tensor(tensor_t *a, float b, tensor_t *outputTensor);
void addFloat32ElementWithFloat32TensorInplace(tensor_t *a, float b);

void addInt32TensorToSymInt32TensorInplace(tensor_t* symInt32Tensor, tensor_t* int32Tensor);
void addFloat32TensorToSymInt32TensorInplace(tensor_t* symInt32Tensor, tensor_t* int32Tensor);
void addSymInt32Tensors(tensor_t* aTensor, tensor_t* bTensor, tensor_t* outputTensor);
void addSymInt32TensorsInplace(tensor_t* aTensor, tensor_t* bTensor);

size_t getAddInstructionCounter();

#endif // ENV5_RUNTIME_ADD_H
