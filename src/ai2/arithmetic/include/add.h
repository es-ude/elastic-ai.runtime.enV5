//
// Created by Leo Buron on 20.10.25.
//

#ifndef ENV5_RUNTIME_ADD_H
#define ENV5_RUNTIME_ADD_H
#include "Tensor.h"

void add_int32(const tensor_t* a, const tensor_t* b, tensor_t* result);

void add_float32(const tensor_t* a, const tensor_t* b, tensor_t* result);

#endif // ENV5_RUNTIME_ADD_H
