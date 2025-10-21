//
// Created by Leo Buron on 21.10.25.
//

#ifndef ENV5_RUNTIME_MUL_H
#define ENV5_RUNTIME_MUL_H

#include "Tensor.h"

void mul_int32(const tensor_t* a, const tensor_t* b, tensor_t* result);

void mul_float32(const tensor_t* a, const tensor_t* b, tensor_t* result);

#endif // ENV5_RUNTIME_MUL_H
