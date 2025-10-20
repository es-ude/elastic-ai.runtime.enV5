//
// Created by Leo Buron on 20.10.25.
//

#include "add.h"
#include "DTypes.h"
uint32_t float_add = 0;

bool doDimensionsMatch(const tensor_t* a, const tensor_t* b) {
    if (a->dimensions != b->dimensions) {
        return false;
    }
    return true;
};

void addTwoTensors(const tensor_t* a, const tensor_t* b, tensor_t* result) {
    if (doDimensionsMatch(a, b)) {
        switch(a->encoding->type) {
        case FLOAT32:

        }
    }
};

float plusFloat(float a, float b) {
    float_add+=1;
    return a + b;
};
