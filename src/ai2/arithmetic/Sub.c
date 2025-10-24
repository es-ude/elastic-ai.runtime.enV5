#include "Sub.h"
#include "Arithmetic.h"

size_t subInstructionCounter = 0;

int32_t subInt32s(int32_t a, int32_t b) {
    ++subInstructionCounter;
    return a - b;
}

float subFloats(float a, float b) {
    ++subInstructionCounter;
    return a - b;
}
void subInt32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    int32ElementArithmeticFunc_t sub = subInt32s;
    int32PointWiseArithmetic(a, b, sub, outputTensor);
}

void subInt32TensorsInplace(tensor_t *a, tensor_t *b) {
    int32ElementArithmeticFunc_t sub = subInt32s;
    int32PointWiseArithmeticInplace(a, b, sub);
}

void subInt32ElementWithTensor(tensor_t *a, int32_t b, tensor_t *outputTensor) {
    int32ElementArithmeticFunc_t sub = subInt32s;
    int32ElementWithTensorArithmetic(a, b, sub, outputTensor);
}

void subInt32ElementWithTensorInplace(tensor_t *a, int32_t b) {
    int32ElementArithmeticFunc_t sub = subInt32s;
    int32ElementWithTensorArithmeticInplace(a, b, sub);
}

void subFloatTensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    floatElementArithmeticFunc_t sub = subFloats;
    floatPointWiseArithmetic(a, b, sub, outputTensor);
}

void subFloatTensorsInplace(tensor_t *a, tensor_t *b) {
    floatElementArithmeticFunc_t sub = subFloats;
    floatPointWiseArithmeticInplace(a, b, sub);
}

void subFloatElementWithTensor(tensor_t *a, float b, tensor_t *outputTensor) {
    floatElementArithmeticFunc_t sub = subFloats;
    floatElementWithTensorArithmetic(a, b, sub, outputTensor);
}

void subFloatElementWithTensorInplace(tensor_t *a, float b) {
    floatElementArithmeticFunc_t sub = subFloats;
    floatElementWithTensorArithmeticInplace(a, b, sub);
}

size_t getSubInstructionCounter() {
    return subInstructionCounter;
}