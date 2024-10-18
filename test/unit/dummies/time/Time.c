#define SOURCE_FILE "DUMMY-TIME-FACADE-LIB"

#include "Time.h"
#include "Common.h"

uint64_t timeUS64(void) {
    PRINT("Call timeUS64");
    return 10;
}

uint32_t timeMS32(void) {
    PRINT("Call timeMS32");
    return 100;
}

