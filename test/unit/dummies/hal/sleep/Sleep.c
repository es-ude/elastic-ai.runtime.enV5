#define SOURCE_FILE "DUMMY-SLEEP-LIB"

#include <stdint.h>

#include "eai/Common.h"
#include "eai/hal/Sleep.h"

void sleep_for_ms(uint32_t msToSleep) {
    // Satisfy the compiler
    PRINT("Call Sleep for %ums", msToSleep);
}

void sleep_for_us(uint64_t usToSleep) {
    // Satisfy the compiler
    PRINT("Call Sleep for %luus", usToSleep);
}
