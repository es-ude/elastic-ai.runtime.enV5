#include "BinaryArithmeticAbstraction.h"


uint32_t leftShift(uint32_t input, uint8_t num) {
    return input<<num;
}

uint32_t rightShift(uint32_t input, uint8_t num) {
    return input>>num;
}

uint32_t rightRotate(uint32_t input, uint8_t num) {
    uint8_t bitlength = sizeof(input) * 8;
    return rightShift(input, num) | leftShift(input, bitlength - num);
}

uint32_t leftRotate(uint32_t input, uint8_t num) {
    uint8_t bitlength = sizeof(input) * 8;
    return leftShift(input, num) | rightShift(input, bitlength -num);
}

uint32_t bitwiseXor(uint32_t inputOne, uint32_t inputTwo){
    return inputOne ^ inputTwo;
}

uint32_t bitwiseNot(uint32_t input){
    return ~input;
}
