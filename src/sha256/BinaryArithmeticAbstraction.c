#include "BinaryArithmeticAbstraction.h"
//#define UINT32_MAX  ((uint32_t)-1)  //habe ich definiert, weil es scheinbar häufig so genutzt
// wird?
                                    //warnung, weil es "redefined" wird
#include <stdbool.h>

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

bool xor(bool inputOne, bool inputTwo){
    return (!inputOne & inputTwo) | (inputOne & !inputTwo);
}

/*  first try:
    uint32_t xor(uint32_t inputOne, uint32_t inputTwo){
    uint32_t negationInputOne = UINT32_MAX-inputOne; //possible without this
    uint32_t negationInputTwo = UINT16_MAX-inputTwo; //and this
    return (negationInputOne & inputTwo) | (inputOne & negationInputTwo);
}*/

uint32_t sigmaZero(uint32_t input){
    return input;
}
uint32_t sigmaOne(uint32_t input){
    return input;
}