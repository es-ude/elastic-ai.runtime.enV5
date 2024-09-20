#define SOURCE_FILE "DUMMY-TIME-FACADE-LIB"

#include "Time.h"
#include "Common.h"

uint64_t timeUS64(void) {
    // Satisfy the compiler
    PRINT("Call timeUS64");
}

uint32_t timeMS32(void) {
    // Satisfy the compiler
    PRINT("Call timeMS32");
}
