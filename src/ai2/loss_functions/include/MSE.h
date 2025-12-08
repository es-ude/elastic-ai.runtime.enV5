#ifndef MSE_H
#define MSE_H
#include "Tensor.h"

void MSELossBackwardFloat(tensor_t *modelOutput, tensor_t *label, tensor_t *result);

void MSELossBackwardAsym(tensor_t *modelOutput, tensor_t *label, tensor_t *result);

void MSELossBackward(tensor_t *modelOutput, tensor_t *label, tensor_t *result);


#endif //MSE_H
