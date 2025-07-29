#include "pico/time.h"

#include "eai/hal/Sleep.h"

void sleep_for_ms(uint32_t msToSleep) {
    sleep_ms(msToSleep);
}

void sleep_for_us(uint64_t usToSleep) {
    sleep_us(usToSleep);
}
