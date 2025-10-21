//
// Created by Leo Buron on 21.10.25.
//

#ifndef ENV5_RUNTIME_SUB_H
#define ENV5_RUNTIME_SUB_H
#include "Tensor.h"

void sub_int32(const tensor_t* a, const tensor_t* b, tensor_t* result);

void sub_float32(const tensor_t* a, const tensor_t* b, tensor_t* result);

#endif // ENV5_RUNTIME_SUB_H
