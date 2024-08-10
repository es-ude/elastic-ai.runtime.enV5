#define SOURCE_FILE "CHECKSUM-XOR"

#include "Checksum.h"

uint8_t checksum(uint8_t input1, uint8_t input2) {
    return input1 ^ input2;
}
