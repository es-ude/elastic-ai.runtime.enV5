#define SOURCE_FILE "DUMMY-TIME-FACADE-LIB"

#include "eai/hal/Time.h"
#include "eai/Common.h"

uint64_t timeUs64(void) {
    PRINT("Call timeUS64");
    return 10;
}

uint32_t timeUs32(void) {
    PRINT("Call timeUS32");
    return 100;
}
