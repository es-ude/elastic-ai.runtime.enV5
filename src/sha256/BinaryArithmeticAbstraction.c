#include "BinaryArithmeticAbstraction.h"


uint32_t leftShift(uint32_t input, uint8_t num) {
    return input<<num;
}

uint32_t rightShift(uint32_t input, uint8_t num) {
    return input>>num;
}

uint32_t rightRotate(uint32_t input, uint8_t num) {
    return rightShift(input, num) | leftShift(input, 32-num);//change for different inputsize
} // later: what does sizeof() do exactly?

uint32_t leftRotate(uint32_t input, uint8_t num) {
    return leftShift(input, num) | rightShift(input, 32-num);//change for different inputsize
}

bool logicalXor(bool inputOne, bool inputTwo){
    return (!inputOne & inputTwo) | (inputOne & !inputTwo);
}

uint32_t bitwiseXor(uint32_t inputOne, uint32_t inputTwo){
    return inputOne ^ inputTwo;
}

uint32_t bitwiseNot(uint32_t input){
    return ~input;
}

uint32_t sigmaZero(uint32_t input){  // a xor b xor c
    return bitwiseXor(bitwiseXor(rightRotate(input,7),
                                        rightRotate(input,18)),
                                        rightShift(input, 3));
}

uint32_t sigmaOne(uint32_t input){  // a xor b xor c
    return bitwiseXor(bitwiseXor(rightRotate(input,17),
                                        rightRotate(input,19)),
                                        rightShift(input, 10));
}

uint32_t sigmaTwo(uint32_t input){  // a xor b xor c ; Σ1
    return bitwiseXor(bitwiseXor(rightRotate(input,6),
                                    rightRotate(input,11)),
                                    rightRotate(input, 25));
}

uint32_t sigmaThree(uint32_t input){  // a xor b xor c ; Σ0
    return bitwiseXor(bitwiseXor(rightRotate(input,2),
                                     rightRotate(input,13)),
                                     rightRotate(input, 22));
}

uint32_t calculationOfWords(uint32_t wZero, uint32_t wOne, uint32_t wNine, uint32_t wFourteen){
    uint32_t wSixteen = wZero + sigmaZero(wOne) + wNine + sigmaOne(wFourteen);
    return wSixteen;
}

uint32_t majority(uint32_t a, uint32_t b, uint32_t c){
    return bitwiseXor(bitwiseXor((a + b), (a + c)), (b + c));
}

uint32_t choice(uint32_t e, uint32_t f, uint32_t g){
    return bitwiseXor((e & f),(bitwiseNot(e) & g));
}

//Sorry, dass ich hier jetzt nicht arithmetischen kram mit rein gepackt habe, aber ich dachte das
// sortieren wir besser morgen gemeinsam, was nochmal separat soll und was
// nicht. Freue mich schon :))