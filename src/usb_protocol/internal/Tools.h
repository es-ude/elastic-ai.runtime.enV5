#ifndef ENV5_TOOLS_HEADER
#define ENV5_TOOLS_HEADER

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//! read arbitrary number of bytes with read-handle
void readBytes(uint8_t *data, size_t numberOfBytes);

//! @brief calculate XOR based checksum of given byte arrays
uint8_t calculateChecksum(int numberOfArguments, va_list data);

//! @brief method to get checksum of arbitrary number of byte arrays
uint8_t getChecksum(int numberOfArguments, ...);

#endif // ENV5_TOOLS_HEADER
