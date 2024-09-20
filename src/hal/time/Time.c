#include "Time.h"

#include "pico/time.h"
#include <stdint.h>

uint64_t timeUs64(void) {
    return (uint64_t)get_absolute_time();
}

uint32_t timeMs32(void) {
    uint64_t time = (uint64_t)get_absolute_time();
    return us_to_ms(time);
}
