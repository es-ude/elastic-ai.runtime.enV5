#include "Sha256.h"
#include "BinaryArithmeticAbstraction.h"

/* Next Steps:
    * in MD5 umbennen
    * Kommentare aufräumen
    * struct erstellen
    * funktion schreiben, die einen String als input bekommt, und diesen String in den struct kopiert 
        * testen ob der string auch korrekt in den struct geschrieben wird
*/

/*
 .Struct enthält: nachricht, die wir verschlüsseln wollen
 * länge der zu verschlüsselnden nachricht
 * initial hashvalues (ggf als eigener Stuct)
 
 
 .Typedef basierend auf Struct
 .funktion, die als Parameter ein Wort bekommt, von dem wir den Hash verechnen wollen. In dieser
 Funktion erstelle ich dann den struct
 */

uint32_t smallSigmaZero(uint32_t input){  // a xor b xor c ; σ0
    return bitwiseXor(bitwiseXor(rightRotate(input,7),
                                        rightRotate(input,18)),
                                        rightShift(input, 3));
}

uint32_t smallSigmaOne(uint32_t input){  // a xor b xor c ; σ1
    return bitwiseXor(bitwiseXor(rightRotate(input,17),
                                        rightRotate(input,19)),
                                        rightShift(input, 10));
}

uint32_t bigSigmaZero(uint32_t input){  // a xor b xor c ; Σ0
    return bitwiseXor(bitwiseXor(rightRotate(input,2),
                                 rightRotate(input,13)),
                      rightRotate(input, 22));
}

uint32_t bigSigmaOne(uint32_t input){  // a xor b xor c ; Σ1
    return bitwiseXor(bitwiseXor(rightRotate(input,6),
                                    rightRotate(input,11)),
                                    rightRotate(input, 25));
}

uint32_t majority(uint32_t a, uint32_t b, uint32_t c){
    return bitwiseXor(bitwiseXor((a & b), (a & c)), (b & c));
}

/* addition > uint32_t !!!
-> https://csrc.nist.gov/csrc/media/publications/fips/180/4/final/documents/fips180-4-draft-aug2014.pdf
page 10 (4.1.2)
 */

uint32_t choice(uint32_t e, uint32_t f, uint32_t g){
    return bitwiseXor((e & f),(bitwiseNot(e) & g));
}

uint32_t additionOfWords(uint32_t a, uint32_t b){
    // double p = pow(2,32); ???
    uint64_t h = 4294967296;
    return (a + b) % h;
}

/*
 page 9 (3.2)
 https://csrc.nist.gov/csrc/media/publications/fips/180/4/final/documents/fips180-4-draft-aug2014.pdf
*/


uint32_t calculationOfWords(
    uint32_t wZero,
    uint32_t wOne,
    uint32_t wNine,
    uint32_t wFourteen){
    uint32_t wSixteen = additionOfWords(
                     additionOfWords(
                     additionOfWords(wZero,
                                        smallSigmaZero(wOne)),
                                        wNine),
                                        smallSigmaOne(wFourteen));
    return wSixteen;
}


//you find some other explanation here   https://sha256algorithm.com/
