#include "TimeFacade.h"

#include "pico/time.h"

uint64_t get_current_time_in_us(void) {
    return (uint64_t)get_absolute_time();
}

uint32_t get_current_time_in_ms(void) {
    uint64_t time = (uint64_t)get_absolute_time();
    return us_to_ms(time);
}
