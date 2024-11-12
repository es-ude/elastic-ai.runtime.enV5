#define SOURCE_FILE "TIME-LIB"

#include <stdint.h>

#include "hardware/timer.h"

/* IMPORTANT: We include as `include/Time.h` to avoid conflict with the pico `time.h`
 * header on case-insensitive platforms (e.g., MacOS).
 */
#include "include/hal/Time.h"

uint64_t timeUs64(void) {
    return time_us_64();
}

uint32_t timeUs32(void) {
    return time_us_32();
}
