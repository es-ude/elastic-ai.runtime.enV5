#include "Mul.h"
#include "Arithmetic.h"

size_t mulInstructionCounter = 0;

int32_t mulInt32s(int32_t a, int32_t b) {
    ++mulInstructionCounter;
    return a * b;
}

float mulFloats(float a, float b) {
    ++mulInstructionCounter;
    return a * b;
}
void mulInt32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    int32ElementArithmeticFunc_t mul = mulInt32s;
    int32PointWiseArithmetic(a, b, mul, outputTensor);
}

void mulInt32TensorsInplace(tensor_t *a, tensor_t *b) {
    int32ElementArithmeticFunc_t mul = mulInt32s;
    int32PointWiseArithmeticInplace(a, b, mul);
}

void mulInt32ElementWithTensor(tensor_t *a, int32_t b, tensor_t *outputTensor) {
    int32ElementArithmeticFunc_t mul = mulInt32s;
    int32ElementWithTensorArithmetic(a, b, mul, outputTensor);
}

void mulInt32ElementWithTensorInplace(tensor_t *a, int32_t b) {
    int32ElementArithmeticFunc_t mul = mulInt32s;
    int32ElementWithTensorArithmeticInplace(a, b, mul);
}

void mulFloatTensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    floatElementArithmeticFunc_t mul = mulFloats;
    floatPointWiseArithmetic(a, b, mul, outputTensor);
}

void mulFloatTensorsInplace(tensor_t *a, tensor_t *b) {
    floatElementArithmeticFunc_t mul = mulFloats;
    floatPointWiseArithmeticInplace(a, b, mul);
}

void mulFloatElementWithTensor(tensor_t *a, float b, tensor_t *outputTensor) {
    floatElementArithmeticFunc_t mul = mulFloats;
    floatElementWithTensorArithmetic(a, b, mul, outputTensor);
}

void mulFloatElementWithTensorInplace(tensor_t *a, float b) {
    floatElementArithmeticFunc_t mul = mulFloats;
    floatElementWithTensorArithmeticInplace(a, b, mul);
}
size_t getMulInstructionCounter() {
    return mulInstructionCounter;
}
