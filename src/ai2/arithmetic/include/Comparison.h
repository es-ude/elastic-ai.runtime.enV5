#ifndef ENV5_RUNTIME_COMPARISONS_H
#define ENV5_RUNTIME_COMPARISONS_H
#include <stdint.h>
#include "Tensor.h"

void GTE_int32Value(tensor_t* a, int32_t b, int32_t altNumber, tensor_t* result);
void GTE_int32Tensor(tensor_t* a, tensor_t* b, int32_t altNumber, tensor_t* result);

void GTE_floatValue(tensor_t* a, float b, float altNumber, tensor_t* result);
void GTE_floatTensor(tensor_t* a, tensor_t* b, float altNumber, tensor_t* result);


#endif // ENV5_RUNTIME_COMPARISONS_H
