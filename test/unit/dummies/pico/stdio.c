#include <stdbool.h>
#include <stdint.h>

void stdio_usb_init() {}
bool stdio_usb_connected() {
    return true;
}
char getchar_timeout_us(uint32_t timeout) {
    return '\0';
}