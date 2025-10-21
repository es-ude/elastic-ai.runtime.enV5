//
// Created by Leo Buron on 20.10.25.
//

#include "Arithmetic.h"

bool doDimensionsMatch(const tensor_t* a, const tensor_t* b) {
    if (a->dimensions != b->dimensions) {
        return false;
    }
    return true;
};