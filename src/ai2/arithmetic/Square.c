#include "Square.h"

size_t squareInstructionCounter = 0;

int32_t squareInt32(int32_t a) {
    ++squareInstructionCounter;
    return a * a;
}

float squareFloat(float a) {
    ++squareInstructionCounter;
    return a * a;
}

size_t getSquareInstructionCounter() {
    return squareInstructionCounter;
}
