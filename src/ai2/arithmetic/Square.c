#include "Square.h"


#ifdef TRACK_INSTRUCTIONS
#define SQUARE_FUNC_INT squareInt32WithInstructionCounter
#define SQUARE_FUNC_FLOAT squareFloatWithInstructionCounter
#else
#define SQUARE_FUNC_INT squareInt
#define SQUARE_FUNC_FLOAT squareFloat
#endif

size_t squareInstructionCounter = 0;

int32_t squareInt(int32_t a) {
    return a * a;
}

int32_t squareIntWithInstructionCounter(int32_t a) {
    ++squareInstructionCounter;
    return a * a;
}

int32_t squareInt32(int32_t a) {
    return SQUARE_FUNC_INT(a);
}

float squareFloat(float a) {
    return a * a;
}

float squareFloatWithInstructionCounter(float a) {
    ++squareInstructionCounter;
    return a * a;
}

float squareFloat32(float a) {
    return SQUARE_FUNC_FLOAT(a);
}

size_t getSquareInstructionCounter() {
    return squareInstructionCounter;
}
