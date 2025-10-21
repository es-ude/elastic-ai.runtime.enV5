//
// Created by Leo Buron on 21.10.25.
//

#ifndef ENV5_RUNTIME_DIV_H
#define ENV5_RUNTIME_DIV_H
#include "Tensor.h"

void div_int32(const tensor_t* a, const tensor_t* b, tensor_t* result);

void div_float32(const tensor_t* a, const tensor_t* b, tensor_t* result);

#endif // ENV5_RUNTIME_DIV_H
