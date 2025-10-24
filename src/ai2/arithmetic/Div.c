#include "Div.h"
#include "Arithmetic.h"

size_t divInstructionCounter = 0;

int32_t divInt32s(int32_t a, int32_t b) {
    ++divInstructionCounter;
    return a / b;
}

float divFloats(float a, float b) {
    ++divInstructionCounter;
    return a / b;
}

void divInt32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    int32ElementArithmeticFunc_t div = divInt32s;
    int32PointWiseArithmetic(a, b, div, outputTensor);
}

void divInt32TensorsInplace(tensor_t *a, tensor_t *b) {
    int32ElementArithmeticFunc_t div = divInt32s;
    int32PointWiseArithmeticInplace(a, b, div);
}

void divInt32ElementWithTensor(tensor_t *a, int32_t b, tensor_t *outputTensor) {
    int32ElementArithmeticFunc_t div = divInt32s;
    int32ElementWithTensorArithmetic(a, b, div, outputTensor);
}

void divInt32ElementWithTensorInplace(tensor_t *a, int32_t b) {
    int32ElementArithmeticFunc_t div = divInt32s;
    int32ElementWithTensorArithmeticInplace(a, b, div);
}

void divFloatTensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    floatElementArithmeticFunc_t div = divFloats;
    floatPointWiseArithmetic(a, b, div, outputTensor);
}

void divFloatTensorsInplace(tensor_t *a, tensor_t *b) {
    floatElementArithmeticFunc_t div = divFloats;
    floatPointWiseArithmeticInplace(a, b, div);
}

void divFloatElementWithTensor(tensor_t *a, float b, tensor_t *outputTensor) {
    floatElementArithmeticFunc_t div = divFloats;
    floatElementWithTensorArithmetic(a, b, div, outputTensor);
}

void divFloatElementWithTensorInplace(tensor_t *a, float b) {
    floatElementArithmeticFunc_t div = divFloats;
    floatElementWithTensorArithmeticInplace(a, b, div);
}

size_t getDivInstructionCounter() {
    return divInstructionCounter;
}