//
// Created by Leo Buron on 21.10.25.
//

#ifndef ENV5_RUNTIME_SQUARE_H
#define ENV5_RUNTIME_SQUARE_H

#include "Tensor.h"

void square_int32(const tensor_t* a, tensor_t* result);

void square_float32(const tensor_t* a, tensor_t* result);

#endif // ENV5_RUNTIME_SQUARE_H
