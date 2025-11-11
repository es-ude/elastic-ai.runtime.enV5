#ifndef ENV5_RUNTIME_COMPARISONS_H
#define ENV5_RUNTIME_COMPARISONS_H
#include <stdint.h>
#include "Tensor.h"

void gteInt32Value(tensor_t* a, int32_t b, int32_t altNumber, tensor_t* result);
void gteInt32Tensor(tensor_t* a, tensor_t* b, int32_t altNumber, tensor_t* result);

void gteFloatValue(tensor_t* a, float b, float altNumber, tensor_t* result);
void gteFloatTensor(tensor_t* a, tensor_t* b, float altNumber, tensor_t* result);

void gteSymInt32Zero(tensor_t *a, int32_t altNumber, tensor_t *result);
//void gteSymInt32Value(tensor_t *a, int32_t b, int32_t altNumber, tensor_t *result);


#endif // ENV5_RUNTIME_COMPARISONS_H
