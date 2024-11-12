#define SOURCE_FILE "DUMMY-TIME-FACADE-LIB"

#include "hal/Time.h"
#include "Common.h"

uint64_t timeUs64(void) {
    PRINT("Call timeUS64");
    return 10;
}

uint32_t timeUs32(void) {
    PRINT("Call timeUS32");
    return 100;
}
