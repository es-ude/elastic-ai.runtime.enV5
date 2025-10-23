//
// Created by Leo Buron on 21.10.25.
//

#ifndef ENV5_RUNTIME_DIV_H
#define ENV5_RUNTIME_DIV_H
#include "Tensor.h"

void div_int32(const int32Tensor_t* a, const int32Tensor_t* b, int32Tensor_t* result);

void div_int32(const int32Tensor_t* a, int32_t b, int32Tensor_t* result);

void div_float32(const float32Tensor_t* a, const float32Tensor_t* b, float32Tensor_t* result);

void div_float32(const float32Tensor_t* a, float b, float32Tensor_t* result);

#endif // ENV5_RUNTIME_DIV_H
