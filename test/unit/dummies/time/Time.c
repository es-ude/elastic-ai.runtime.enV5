#define SOURCE_FILE "DUMMY-TIME-LIB"

#include "Common.h"
#include <stdint.h>

void sleep_for_ms(uint32_t msToSleep) {
    // Satisfy the compiler
    PRINT("Call Sleep for %li ms", msToSleep);
}
