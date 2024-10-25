#define SOURCE_FILE "TIME-LIB"

#include <stdint.h>

#include "hardware/timer.h"

#include "Time.h"

uint64_t timeUs64(void) {
    return time_us_64();
}

uint32_t timeUs32(void) {
    return time_us_32();
}
