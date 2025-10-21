//
// Created by Leo Buron on 21.10.25.
//

#ifndef ENV5_RUNTIME_COMPARISONS_H
#define ENV5_RUNTIME_COMPARISONS_H
#include <stdint.h>
#include "Tensor.h"

void GTE_int32(const int32Tensor_t* a, int32_t b, int32_t altNumber, int32Tensor_t* result); // A>=B für alle die das Falsch ist, wird altNumber genommen - Hierrüber sollten wir nochmal reden, wenn du soweit bist

void GTE_float32(const float32Tensor_t* a, float b, float altNumber, float32Tensor_t* result); //siehe oben

#endif // ENV5_RUNTIME_COMPARISONS_H
