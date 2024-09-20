#define SOURCE_FILE "DUMMY-TIME-FACADE-LIB"

#include "Time.h"
#include "Common.h"

uint64_t get_current_time_in_us(void) {
    // Satisfy the compiler
    PRINT("Call get_current_time_in_us");
}

uint32_t get_current_time_in_ms(void) {
    // Satisfy the compiler
    PRINT("Call get_current_time_in_ms");
}
