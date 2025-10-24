//
// Created by Leo Buron on 21.10.25.
//
#include "Linear.h"

#include "Add.h"
#include "Matmul.h"
#include "Mul.h"
#include "Tensor.h"

void linearForwardFloat(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {
    matmul_float32(w, input, output);
    addFloatTensorsInplace(output, b);
}

void linearForwardLinearQ(tensor_t *w, tensor_t *b, tensor_t *input, tensor_t *output) {
    /* PSEUDOCode
     * intermediate_int = int(w) * int(input)
     * intermediate_int += bias;
     * if output->qtype == FLOAT
     * {
     * output = float(intermediate_int);
     * output *= w->qconfig->scale * input->qConfig->scale;
     * }
     * else if output->qType == LINEAR
     * {
     * output = linearQ(intermediate_int)
     * output->qconfig->scale *= w->qconfig->scale * input->qConfig->scale;
     * }
     */
    quantization_t intermediate_int_quantization;
    initInt32Quantization(&intermediate_int_quantization);
    uint8_t intermediate_int_data[calcNumberOfElementsByTensor(output) * calcBytesPerElement(
                                      &intermediate_int_quantization)];
    tensor_t intermediate_int = {.data = intermediate_int_data,
                                 .quantization = &intermediate_int_quantization,};
    {
        quantization_t w_int_quantization;
        initInt32Quantization(&w_int_quantization);
        uint8_t w_int_data[calcNumberOfElementsByTensor(w) * calcBytesPerElement(
                               &w_int_quantization)];
        tensor_t w_int = {.data = w_int_data, .quantization = &w_int_quantization,};
        convertTensor(w, &w_int);

        quantization_t input_int_quantization;
        initInt32Quantization(&input_int_quantization);
        uint8_t input_int_data[calcNumberOfElementsByTensor(input) * calcBytesPerElement(
                                   &input_int_quantization)];
        tensor_t input_int = {.data = input_int_data, .quantization = &input_int_quantization,};
        convertTensor(input, &input_int);

        matmul_int32(&w_int, &input_int, &intermediate_int);
    }
    addInt32TensorsInplace(&intermediate_int, b);

    linearQConfig_t *linearQWConfig = (linearQConfig_t *)w->quantization->qConfig;
    linearQConfig_t *linearQInputConfig = (linearQConfig_t *)input->quantization->qConfig;
    if (output->quantization->type == FLOAT32) {
        quantization_t intermediate_float_quantization;
        initInt32Quantization(&intermediate_float_quantization);
        uint8_t intermediate_float_data[calcNumberOfElementsByTensor(input) * calcBytesPerElement(
                                            &intermediate_float_quantization)];
        tensor_t intermediate_float = {.data = intermediate_float_data,
                                       .quantization = &intermediate_float_quantization,};
        convertTensor(&intermediate_int, output);
        mul_float32Inplace(output, linearQWConfig->scale * linearQInputConfig->scale);
    } else if (output->quantization->type == LINEAR) {
        convertTensor(&intermediate_int, output);
        linearQConfig_t *linearQOutConfig = (linearQConfig_t *)output->quantization->qConfig;
        linearQOutConfig->scale *= linearQWConfig->scale * linearQInputConfig->scale;
    }
}


void linearForward(void *config, tensor_t *input, tensor_t *output) {
    /*PSEUDO-Code:
     *Convert Params W und B to Tensors
     *if lConfig is a type FLOATLayer go there
     *else go to linearlayer
     */
    linearConfig_t *lConfig = (linearConfig_t *)&config;
    tensor_t weights;
    size_t orderOfDimensionsWeights[lConfig->weight->numberOfDimensions];
    getTensorFromParameter(lConfig->weight, &weights, orderOfDimensionsWeights);
    tensor_t bias;
    size_t orderOfDimensionsBias[lConfig->bias->numberOfDimensions];
    getTensorFromParameter(lConfig->bias, &bias, orderOfDimensionsBias);
    if (lConfig->type == FLOATLAYER) {
        linearForwardFloat(&weights, &bias, input, output);

    } else if (lConfig->type == LINEARLAYER) {
        linearForwardLinearQ(&weights, &bias, input, output);

    }

}
