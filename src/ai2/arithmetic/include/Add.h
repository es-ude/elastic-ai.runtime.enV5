//
// Created by Leo Buron on 20.10.25.
//

#ifndef ENV5_RUNTIME_ADD_H
#define ENV5_RUNTIME_ADD_H
#include "Tensor.h"

void add_int32(const int32Tensor_t* a, const int32Tensor_t* b, int32Tensor_t* result);

void add_float32(const float32Tensor_t* a, const float32Tensor_t* b, float32Tensor_t* result);

#endif // ENV5_RUNTIME_ADD_H
