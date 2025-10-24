#include "Add.h"
#include "Tensor.h"
#include "DTypes.h"
#include "Arithmetic.h"

#include <stdio.h>
#include <stdlib.h>

size_t addInstructionCounter = 0;

float addFloats(float a, float b) {
    ++addInstructionCounter;
    return a + b;
}

int32_t addInt32s(int32_t a, int32_t b) {
    ++addInstructionCounter;
    return a + b;
}

void addInt32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    int32ElementArithmeticFunc_t add = addInt32s;
    int32PointWiseArithmetic(a, b, add, outputTensor);
}

void addInt32TensorsInplace(tensor_t *a, tensor_t *b) {
    int32ElementArithmeticFunc_t add = addInt32s;
    int32PointWiseArithmeticInplace(a, b, add);
}

void addInt32ElementWithTensor(tensor_t *a, int32_t b, tensor_t *outputTensor) {
    int32ElementArithmeticFunc_t add = addInt32s;
    int32ElementWithTensorArithmetic(a, b, add, outputTensor);
}

void addInt32ElementWithTensorInplace(tensor_t *a, int32_t b) {
    int32ElementArithmeticFunc_t add = addInt32s;
    int32ElementWithTensorArithmeticInplace(a, b, add);
}

void addFloatTensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    floatElementArithmeticFunc_t add = addFloats;
    floatPointWiseArithmetic(a, b, add, outputTensor);
}

void addFloatTensorsInplace(tensor_t *a, tensor_t *b) {
    floatElementArithmeticFunc_t add = addFloats;
    floatPointWiseArithmeticInplace(a, b, add);
}

void addFloatElementWithTensor(tensor_t *a, float b, tensor_t *outputTensor) {
    floatElementArithmeticFunc_t add = addFloats;
    floatElementWithTensorArithmetic(a, b, add, outputTensor);
}

void addFloatElementWithTensorInplace(tensor_t *a, float b) {
    floatElementArithmeticFunc_t add = addFloats;
    floatElementWithTensorArithmeticInplace(a, b, add);
}

void addTensorsInplace(tensor_t *a, tensor_t *b) {
    qtype_t aType = a->quantization->type;
    qtype_t bType = b->quantization->type;

    if (aType != bType) {
        printf("Error in addTensors: mismatched tensor qtypes");
    }

    switch (aType) {
    case FLOAT32:
        addFloatTensorsInplace(a, b);
        break;
    case INT32:
        addInt32TensorsInplace(a, b);
        break;
    default:
        printf("Error in addTensors: unsupported tensor qtype");
    }
}

size_t getAddInstructionCounter() {
    return addInstructionCounter;
}
