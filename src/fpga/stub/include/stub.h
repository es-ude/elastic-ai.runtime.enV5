#ifndef STUB_HEADER
#define STUB_HEADER

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool modelDeploy(uint32_t acceleratorAddress, const uint8_t *acceleratorId);

void modelPredict(int8_t *inputs, size_t num_inputs, int8_t *result, size_t num_results);

void modelGetId(uint8_t *id);

#endif
