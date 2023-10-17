#ifndef FLOW_PREDICTION_STUB_H
#define FLOW_PREDICTION_STUB_H

#include <stdbool.h>
#include <stdint.h>

bool flow_prediction_deploy(void);
int8_t flow_prediction_predict(int8_t *inputs);

#endif
