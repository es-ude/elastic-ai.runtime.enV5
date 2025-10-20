//
// Created by Leo Buron on 20.10.25.
//

#ifndef ENV5_RUNTIME_ADD_H
#define ENV5_RUNTIME_ADD_H
#include "Tensor.h"

#include <stdbool.h>

bool doDimensionsMatch(const tensor_t* a, const tensor_t* b);

void add(const tensor_t* a, const tensor_t* b, tensor_t* result);

float plusFloat(float a, float b);

#endif // ENV5_RUNTIME_ADD_H
