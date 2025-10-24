#include "Linear.h"
#include "Tensor.h"
#include "unity.h"

#include <DTypes.h>

void setUp() {}
void tearDown() {}

linearConfig_t initLinearConfigFloat() {
    float weightData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float weightGrads[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;

    quantization_t weightQ = {
        .type = FLOAT32
    };
    quantization_t weightGradQ = {
        .type = FLOAT32
    };

    parameter_t weights = {
        .data = weightData,
        .dimensions = weightDims,
        .grad = weightGrads,
        .dataQuantization = &weightQ,
        .gradQuantization = &weightGradQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims
    };

    float biasData[] = {-1.f, 3.f};
    float biasGrads[] = {0.f, 0.f};
    size_t biasDims[] = {2};
    size_t biasNumberOfDims = 1;

    quantization_t biasQ = {
        .type = FLOAT32
    };
    quantization_t biasGradQ = {
        .type = FLOAT32
    };

    parameter_t bias = {
        .data = biasData,
        .dimensions = biasDims,
        .grad = biasGrads,
        .dataQuantization = &biasQ,
        .gradQuantization = &biasGradQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims
    };


    linearConfig_t linearConfig;
    linearConfig.bias = &bias;
    linearConfig.weight = &weights;
    linearConfig.type = FLOATLAYER;

    return linearConfig;
}

void testLinearForwardFloat() {
    float weightData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    float weightGrads[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;

    quantization_t weightQ = {
        .type = FLOAT32
    };
    quantization_t weightGradQ = {
        .type = FLOAT32
    };

    parameter_t weights = {
        .data = weightData,
        .dimensions = weightDims,
        .grad = weightGrads,
        .dataQuantization = &weightQ,
        .gradQuantization = &weightGradQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims
    };

    float biasData[] = {-1.f, 3.f};
    float biasGrads[] = {0.f, 0.f};
    size_t biasDims[] = {2};
    size_t biasNumberOfDims = 1;

    quantization_t biasQ = {
        .type = FLOAT32
    };
    quantization_t biasGradQ = {
        .type = FLOAT32
    };

    parameter_t bias = {
        .data = biasData,
        .dimensions = biasDims,
        .grad = biasGrads,
        .dataQuantization = &biasQ,
        .gradQuantization = &biasGradQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims
    };


    linearConfig_t linearConfig;
    linearConfig.bias = &bias;
    linearConfig.weight = &weights;
    linearConfig.type = FLOATLAYER;

    float inputData[] = {0.f, 1.f, 2.f};
    size_t inputDims[] = {3};
    size_t inputNumberOfDims = 1;
    size_t inputOrderOfDims = {0};
    quantization_t inputQ = {
        .type = FLOAT32
    };

    tensor_t input = {
        .data = inputData,
        .dimensions = inputDims,
        .numberOfDimensions = inputNumberOfDims,
        .sparsityBitmask = NULL,
        .quantization = &inputQ,
        .orderOfDimensions = &inputOrderOfDims
    };

    float outputData[2] = {0, 0};
    size_t outputDims[] = {2};
    size_t outputNumberOfDims = 1;
    size_t outputOrderOfDims = {0};
    quantization_t outputQ = {
        .type = FLOAT32
    };

    tensor_t output = {
        .data = outputData,
        .dimensions = outputDims,
        .numberOfDimensions = outputNumberOfDims,
        .sparsityBitmask = NULL,
        .quantization = &outputQ,
        .orderOfDimensions = &outputOrderOfDims
    };

    linearForward(&linearConfig, &input, &output);

    float expected[] = {-5.f, -4.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, output.data, 2);
}

void testLinearForwardLinear() {
   size_t numberOfWeights = 6;
    size_t numberOfBiases = 2;

    float weightFloatData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    /*
    -1,  4
     2,  5
    -3, -6
    */

    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;
    size_t weightOrderOfDims[] = {0, 1};


    quantization_t weightFloatQ = {
        .type = FLOAT32
    };


    tensor_t weightsFloat = {
        .data = weightFloatData,
        .dimensions = weightDims,
        .quantization = &weightFloatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims,
        .orderOfDimensions = weightOrderOfDims
    };

    uint8_t weightLinearData[6];
    linearQConfig_t weightsLinearQConfig;
    initLinearQConfig(8, HTE, &weightsLinearQConfig);

    quantization_t weightLinearQ = {
        .type = LINEAR,
        .qConfig = &weightsLinearQConfig
    };

    tensor_t weightsLinear = {
        .data = weightLinearData,
        .dimensions = weightDims,
        .quantization = &weightLinearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims,
        .orderOfDimensions = weightOrderOfDims
    };


    convertTensor(&weightsFloat, &weightsLinear);


    uint8_t weightGrads[] = {0, 0, 0, 0, 0, 0};

    parameter_t weights = {
        .data = weightLinearData,
        .dimensions = weightDims,
        .grad = weightGrads,
        .dataQuantization = &weightLinearQ,
        // TODO WROOOOOONG
        .gradQuantization = &weightLinearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims
    };

    float biasFloatData[] = {-1.f, 3.f};
    size_t biasDims[] = {2, 1};
    size_t biasNumberOfDims = 2;
    size_t biasOrderOfDims[] = {0, 1};


    quantization_t biasFloatQ = {
        .type = FLOAT32
    };

    tensor_t biasFloat = {
        .data = biasFloatData,
        .dimensions = biasDims,
        .quantization = &biasFloatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims,
        .orderOfDimensions = biasOrderOfDims
    };

    uint8_t biasLinearData[numberOfBiases];
    linearQConfig_t biasLinearQConfig;
    initLinearQConfig(8, HTE, &biasLinearQConfig);

    quantization_t biasLinearQ = {.type = LINEAR, .qConfig = &biasLinearQConfig};
    tensor_t biasLinear = {
        .data = biasLinearData,
        .dimensions = biasDims,
        .quantization = &biasLinearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims,
        .orderOfDimensions = biasOrderOfDims
    };

    convertTensor(&biasFloat,&biasLinear);

    uint8_t biasGrads[numberOfBiases];

    parameter_t bias = {
        .data = biasLinearData,
        .dimensions = biasDims,
        .grad = biasGrads,
        .dataQuantization = &biasLinearQ,
        .gradQuantization = &biasLinearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims
    };

    linearConfig_t linearConfig;
    linearConfig.bias = &bias;
    linearConfig.weight = &weights;
    linearConfig.type = LINEARLAYER;







    float inputFloatData[] = {0.f, 1.f, 2.f};
    size_t inputDims[] = {3};
    size_t inputNumberOfDims = 1;
    size_t inputOrderOfDims[] = {0};
    quantization_t inputFloatQ = {
        .type = FLOAT32
    };

    tensor_t inputFloat = {
        .data = inputFloatData,
        .dimensions = inputDims,
        .numberOfDimensions = inputNumberOfDims,
        .sparsityBitmask = NULL,
        .quantization = &inputFloatQ,
        .orderOfDimensions = inputOrderOfDims
    };

    uint8_t inputLinearData[3];
    linearQConfig_t inputLinearQConfig;
    initLinearQConfig(8, HTE, &inputLinearQConfig);
    quantization_t inputLinearQ = {.type = LINEAR, .qConfig = &inputLinearQConfig};

    tensor_t inputLinear = {
        .data = inputLinearData,
        .dimensions = inputDims,
        .quantization = &inputLinearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = inputNumberOfDims,
        .orderOfDimensions = inputOrderOfDims
    };

    convertTensor(&inputFloat, &inputLinear);



    size_t outputDims[] = {2, 1};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};

    uint8_t outputLinearData[2];
    linearQConfig_t outputLinearQConfig;
    initLinearQConfig(8, HTE, &outputLinearQConfig);
    quantization_t outputLinearQ = {.type = FLOAT32/*, .qConfig = &outputLinearQConfig*/};

    tensor_t outputLinear = {
        .data = outputLinearData,
        .dimensions = outputDims,
        .quantization = &outputLinearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = outputNumberOfDims,
        .orderOfDimensions = outputOrderOfDims
    };


    linearForward(&linearConfig, &inputLinear, &outputLinear);

    float output[2];
    readBytesAsFloatArray(2, outputLinear.data, output);

    for(size_t i = 0; i < 2; i++) {
        printf("%f\n", output[i]);
    }

    float expected[] = {-5.f, -4.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, outputLinear.data, 2);

}

void testLinearBackwardFloat() {
    float weightData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    /*
    -1,  4
     2,  5
    -3, -6
    */

    float weightGrads[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;

    quantization_t weightQ = {
        .type = FLOAT32
    };
    quantization_t weightGradQ = {
        .type = FLOAT32
    };

    parameter_t weights = {
        .data = weightData,
        .dimensions = weightDims,
        .grad = weightGrads,
        .dataQuantization = &weightQ,
        .gradQuantization = &weightGradQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims
    };

    float biasData[] = {-1.f, 3.f};
    float biasGrads[] = {0.f, 0.f};
    size_t biasDims[] = {2, 1};
    size_t biasNumberOfDims = 2;

    quantization_t biasQ = {
        .type = FLOAT32
    };
    quantization_t biasGradQ = {
        .type = FLOAT32
    };

    parameter_t bias = {
        .data = biasData,
        .dimensions = biasDims,
        .grad = biasGrads,
        .dataQuantization = &biasQ,
        .gradQuantization = &biasGradQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims
    };


    linearConfig_t linearConfig;
    linearConfig.bias = &bias;
    linearConfig.weight = &weights;
    linearConfig.type = FLOATLAYER;

    float output[] = {0.f, 1.f, 2.f};
    size_t outputDims[] = {3, 1};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};

    quantization_t outputQ = {
        .type = FLOAT32
    };

    tensor_t outputTensor = {
        .data = output,
        .dimensions = outputDims,
        .quantization = &outputQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = outputNumberOfDims,
        .orderOfDimensions = outputOrderOfDims
    };

    float loss[] = {-4.f, -3.f};
    size_t lossDims[] = {2, 1};
    size_t lossNumberOfDims = 2;
    size_t lossOrderOfDims[] = {0, 1};

    quantization_t lossQ = {
        .type = FLOAT32
    };

    tensor_t lossTensor = {
        .data = loss,
        .dimensions = lossDims,
        .quantization = &lossQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = lossNumberOfDims,
        .orderOfDimensions = lossOrderOfDims
    };

    float propLoss[3];
    size_t propLossDims[] = {3};
    size_t propLossNumberOfDims = 1;
    size_t propLossOrderOfDims[] = {0};

    quantization_t propLossQ = {
        .type = FLOAT32
    };


    tensor_t propLossTensor = {
        .data = propLoss,
        .dimensions = propLossDims,
        .quantization = &propLossQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = propLossNumberOfDims,
        .orderOfDimensions = propLossOrderOfDims
    };
    linearBackward(&linearConfig, &lossTensor, &outputTensor, &propLossTensor);


    float expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    float expected_bias_grad[] = {-4.f, -3.f};

    float propLossOutput[3];
    readBytesAsFloatArray(3, propLossTensor.data, propLossOutput);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propLossTensor.data,
                                  sizeof(expected_propagated_loss) / sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig.weight->grad,
                                  calcNumberOfElementsByDims(linearConfig.weight->numberOfDimensions, linearConfig.weight->dimensions));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad, linearConfig.bias->grad,
                                  calcNumberOfElementsByDims(linearConfig.bias->numberOfDimensions, linearConfig.bias->dimensions));


}

void testLinearBackwardLinear() {

    size_t numberOfWeights = 6;
    size_t numberOfBiases = 2;

    float weightFloatData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    /*
    -1,  4
     2,  5
    -3, -6
    */

    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;
    size_t weightOrderOfDims[] = {0, 1};

    quantization_t weightFloatQ = {
        .type = FLOAT32
    };

    tensor_t weightsFloat = {
        .data = weightFloatData,
        .dimensions = weightDims,
        .quantization = &weightFloatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims,
        .orderOfDimensions = weightOrderOfDims
    };

    uint8_t weightLinearData[6];
    linearQConfig_t weightsLinearQConfig;
    initLinearQConfig(8, HTE, &weightsLinearQConfig);
    quantization_t weightLinearQ = {
        .type = LINEAR,
        .qConfig = &weightsLinearQConfig
    };
    tensor_t weightsLinear = buildTensorForConversion(weightLinearData, &weightLinearQ, &weightsFloat);
    convertTensor(&weightsFloat, &weightsLinear);


    uint8_t weightGrads[] = {0, 0, 0, 0, 0, 0};
    linearQConfig_t weightGradsLinearQConfig;
    initLinearQConfig(8, HTE, &weightGradsLinearQConfig);

    quantization_t weightGradsLinearQ = {
        .type = LINEAR,
        .qConfig = &weightGradsLinearQConfig
    };

    parameter_t weights = {
        .data = weightLinearData,
        .dimensions = weightDims,
        .grad = weightGrads,
        .dataQuantization = &weightLinearQ,
        .gradQuantization = &weightGradsLinearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims
    };

    float biasFloatData[] = {-1.f, 3.f};
    size_t biasDims[] = {2, 1};
    size_t biasNumberOfDims = 2;
    size_t biasOrderOfDims[] = {0, 1};

    quantization_t biasFloatQ = {
        .type = FLOAT32
    };

    tensor_t biasFloat = {
        .data = biasFloatData,
        .dimensions = biasDims,
        .quantization = &biasFloatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims,
        .orderOfDimensions = biasOrderOfDims
    };

    uint8_t biasLinearData[numberOfBiases];
    linearQConfig_t biasLinearQConfig;
    initLinearQConfig(8, HTE, &biasLinearQConfig);
    quantization_t biasLinearQ = {.type = LINEAR, .qConfig = &biasLinearQConfig};
    tensor_t biasLinear = buildTensorForConversion(biasLinearData, &biasLinearQ, &biasFloat);
    convertTensor(&biasFloat,&biasLinear);

    uint8_t biasGrads[numberOfBiases];
    linearQConfig_t biasGradsLinearQConfig;
    initLinearQConfig(8, HTE, &biasGradsLinearQConfig);
    quantization_t biasGradsLinearQ = {.type = LINEAR, .qConfig = &biasGradsLinearQConfig};

    parameter_t bias = {
        .data = biasLinearData,
        .dimensions = biasDims,
        .grad = biasGrads,
        .dataQuantization = &biasLinearQ,
        .gradQuantization = &biasGradsLinearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims
    };


    linearConfig_t linearConfig;
    linearConfig.bias = &bias;
    linearConfig.weight = &weights;
    linearConfig.type = LINEARLAYER;

    float outputFloatData[] = {0.f, 1.f, 2.f};
    size_t outputDims[] = {3, 1};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};

    quantization_t outputFloatQ = {
        .type = FLOAT32
    };

    tensor_t outputFloat = {
        .data = outputFloatData,
        .dimensions = outputDims,
        .quantization = &outputFloatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = outputNumberOfDims,
        .orderOfDimensions = outputOrderOfDims
    };

    uint8_t outputLinearData[3];
    linearQConfig_t outputLinearQConfig;
    initLinearQConfig(8, HTE, &outputLinearQConfig);
    quantization_t outputLinearQ;
    initLinearQuantization(&outputLinearQConfig, &outputLinearQ);
    tensor_t outputLinear = buildTensorForConversion(outputLinearData, &outputLinearQ, &outputFloat);
    convertTensor(&outputFloat, &outputLinear);

    float lossFloatData[] = {-4.f, -3.f};
    size_t lossDims[] = {2, 1};
    size_t lossNumberOfDims = 2;
    size_t lossOrderOfDims[] = {0, 1};

    quantization_t lossFloatQ = {
        .type = FLOAT32
    };

    tensor_t lossFloat = {
        .data = lossFloatData,
        .dimensions = lossDims,
        .quantization = &lossFloatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = lossNumberOfDims,
        .orderOfDimensions = lossOrderOfDims
    };

    uint8_t lossLinearData[3];
    linearQConfig_t lossLinearQConfig;
    initLinearQConfig(8, HTE, &lossLinearQConfig);
    quantization_t lossLinearQ;
    initLinearQuantization(&lossLinearQConfig, &lossLinearQ);
    tensor_t lossLinear = buildTensorForConversion(lossLinearData, &lossLinearQ, &lossFloat);
    convertTensor(&lossFloat, &lossLinear);


    uint8_t propLossLinearData[3];
    size_t propLossDims[] = {3};
    size_t propLossNumberOfDims = 1;
    size_t propLossOrderOfDims[] = {0};

    linearQConfig_t propLossLinearQConfig;
    initLinearQConfig(8, HTE, &propLossLinearQConfig);
    quantization_t propLossLinearQ;
    initLinearQuantization(&propLossLinearQConfig, &propLossLinearQ);

    tensor_t propLossLinear = {
        .data = propLossLinearData,
        .dimensions = propLossDims,
        .quantization = &propLossLinearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = propLossNumberOfDims,
        .orderOfDimensions = propLossOrderOfDims
    };

    linearBackward(&linearConfig, &lossLinear, &outputLinear, &propLossLinear);

    uint8_t weightGradsLinearData[6];
    size_t weightGradsFloatOrderOfDimensions[] = {0, 1};

    tensor_t weightGradsLinear = {
        .data = weightGradsLinearData,
        .dimensions = weightDims,
        .quantization = &weightGradsLinearQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims,
        .orderOfDimensions = weightOrderOfDims
    };

    getGradTensorFromParameter(linearConfig.weight, &weightGradsLinear, weightGradsFloatOrderOfDimensions);

    for(size_t i = 0; i < numberOfWeights; i++) {
        printf("output linear: %i\n", linearConfig.weight->grad[i]);
    }
    linearQConfig_t *te = linearConfig.weight->gradQuantization->qConfig;
    printf("output linear scale: %f\n", te->scale);
    printf("output linear zero point: %i\n", te->zeroPoint);


    float weightGradFloatData[6];
    quantization_t weightGradFloatQ;
    initFloat32Quantization(&weightGradFloatQ);
    tensor_t weightGradsFloat = buildTensorForConversion(weightGradFloatData, &weightGradFloatQ, &propLossLinear);

    convertTensor(&weightGradsLinear, &weightGradsFloat);

    size_t numberOfElements = 6;
    float weightGrad[numberOfElements];
    readBytesAsFloatArray(numberOfElements, weightGradsFloat.data, weightGrad);
    for(size_t i = 0; i < numberOfElements; i++) {
        printf("%f\n", weightGrad[i]);
    }


    float expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    float expected_bias_grad[] = {-4.f, -3.f};

    float propLossOutput[3];
    readBytesAsFloatArray(3, propLossLinear.data, propLossOutput);

    /*TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propLossTensor.data,
                                  sizeof(expected_propagated_loss) / sizeof(float));*/
    /*
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig.weight->grad,
                                  calcNumberOfElementsByDims(linearConfig.weight->numberOfDimensions, linearConfig.weight->dimensions));

    */
    /*
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad, linearConfig.bias->grad,
                                  calcNumberOfElementsByDims(linearConfig.bias->numberOfDimensions, linearConfig.bias->dimensions));
                                  */


}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testLinearForwardLinear);
    /*RUN_TEST(testLinearForwardFloat);
    RUN_TEST(testLinearBackwardFloat);*/
    RUN_TEST(testLinearBackwardLinear);
    UNITY_END();
}
