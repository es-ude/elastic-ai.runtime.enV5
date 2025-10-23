#ifndef ELASTIC_AI_RUNTIME_ENV5_DTYPES_H
#define ELASTIC_AI_RUNTIME_ENV5_DTYPES_H

#include "Rounding.h"
#include <stddef.h>
#include <stdint.h>

int32_t readBytesAsInt32(uint8_t *bytes);
float readBytesAsFloat(uint8_t *bytes);

#endif // ELASTIC_AI_RUNTIME_ENV5_DTYPES_H
