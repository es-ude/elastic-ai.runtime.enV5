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

void testLinearForwardAsym() {
    size_t numberOfWeights = 6;
    size_t numberOfBiases = 2;
    size_t numberOfInputs = 3;
    size_t numberOfOutputs = 2;

    float weightFloatData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};

    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;
    size_t weightOrderOfDims[] = {0, 1};

    quantization_t weightFloatQ;
    initFloat32Quantization(&weightFloatQ);

    tensor_t weightsFloat;
    setTensorValues(&weightsFloat, weightFloatData, weightDims, weightNumberOfDims, weightOrderOfDims, &weightFloatQ, NULL);

    uint8_t weightAsymData[numberOfWeights];
    asymQConfig_t weightsAsymQConfig;
    initAsymQConfig(8, HTE, &weightsAsymQConfig);
    quantization_t weightAsymQ;
    initAsymQuantization(&weightsAsymQConfig, &weightAsymQ);

    tensor_t weightsAsym;
    setTensorValuesForConversion(weightAsymData, &weightAsymQ, &weightsFloat, &weightsAsym);
    convertTensor(&weightsFloat, &weightsAsym);

    uint8_t weightGrads[numberOfWeights];
    asymQConfig_t weightGradsAsymQConfig;
    initAsymQConfig(8, HTE, &weightGradsAsymQConfig);
    quantization_t weightGradsAsymQ;
    initAsymQuantization(&weightGradsAsymQConfig, &weightGradsAsymQ);

    parameter_t weights = {
        .data = weightAsymData,
        .dimensions = weightDims,
        .grad = weightGrads,
        .dataQuantization = &weightAsymQ,
        .gradQuantization = &weightGradsAsymQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims
    };


    int32_t biasIntData[] = {-2979, 8937};
    size_t biasDims[] = {2, 1};
    size_t biasNumberOfDims = 2;
    size_t biasOrderOfDims[] = {0, 1};

    quantization_t biasIntQ;
    initInt32Quantization(&biasIntQ);

    tensor_t biasInt;
    setTensorValues(&biasInt, biasIntData, biasDims, biasNumberOfDims, biasOrderOfDims, &biasIntQ, NULL);

    int32_t biasGrads[numberOfBiases];
    quantization_t biasGradsQ;
    initInt32Quantization(&biasGradsQ);

    parameter_t bias = {
        .data = biasIntData,
        .dimensions = biasDims,
        .grad = biasGrads,
        .dataQuantization = &biasIntQ,
        .gradQuantization = &biasGradsQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims
    };



    linearConfig_t linearConfig;
    linearConfig.bias = &bias;
    linearConfig.weight = &weights;
    linearConfig.type = ASYMLAYER;



    float inputFloatData[] = {0.f, 1.f, 2.f};
    size_t inputDims[] = {3};
    size_t inputNumberOfDims = 1;
    size_t inputOrderOfDims[] = {0};
    quantization_t inputFloatQ;
    initFloat32Quantization(&inputFloatQ);

    tensor_t inputFloat;
    setTensorValues(&inputFloat, inputFloatData, inputDims, inputNumberOfDims, inputOrderOfDims, &inputFloatQ, NULL);

    asymQConfig_t inputAsymQConfig;
    initAsymQConfig(8, HTE, &inputAsymQConfig);
    quantization_t inputAsymQ;
    initAsymQuantization(&inputAsymQConfig, &inputAsymQ);
    uint8_t inputAsymData[numberOfInputs * calcBytesPerElement(&inputAsymQ)];

    tensor_t inputAsym;
    setTensorValuesForConversion(inputAsymData, &inputAsymQ, &inputFloat, &inputAsym);
    convertTensor(&inputFloat, &inputAsym);

    size_t outputDims[] = {2, 1};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};

    asymQConfig_t outputAsymQConfig;
    initAsymQConfig(8, HTE, &outputAsymQConfig);
    quantization_t outputAsymQ;
    initAsymQuantization(&outputAsymQConfig, &outputAsymQ);
    uint8_t outputAsymData[numberOfOutputs * calcBytesPerElement(&outputAsymQ)];


    tensor_t outputAsym;
    setTensorValues(&outputAsym, outputAsymData, outputDims, outputNumberOfDims, outputOrderOfDims, &outputAsymQ, NULL);

    linearForward(&linearConfig, &inputAsym, &outputAsym);

    float outputFloatData[numberOfOutputs];
    quantization_t outputFloatQ;
    initFloat32Quantization(&outputFloatQ);
    tensor_t outputFloat;
    setTensorValuesForConversion(outputFloatData, &outputFloatQ, &outputAsym, &outputFloat);
    convertTensor(&outputAsym, &outputFloat);

    float actual[numberOfOutputs];
    readBytesAsFloatArray(numberOfOutputs, outputFloat.data, actual);

    float expected[] = {-5.f, -4.f};

    for(size_t i = 0; i < numberOfOutputs; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.1f, expected[i], actual[i]);
    }
}

void testLinearBackwardFloat() {
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
                                  calcNumberOfElementsByDims(linearConfig.weight->numberOfDimensions
                                      , linearConfig.weight->dimensions));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad, linearConfig.bias->grad,
                                  calcNumberOfElementsByDims(linearConfig.bias->numberOfDimensions,
                                      linearConfig.bias->dimensions));

}

void testLinearBackwardAsym() {

    size_t numberOfWeights = 6;
    size_t numberOfBiases = 2;
    size_t numberOfLosses = 2;
    size_t numberOfForwardInputs = 3;

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

    asymQConfig_t weightsAsymQConfig;
    initAsymQConfig(8, HTE, &weightsAsymQConfig);
    quantization_t weightsAsymQ;
    initAsymQuantization(&weightsAsymQConfig, &weightsAsymQ);
    uint8_t weightsAsymData[numberOfWeights * calcBytesPerElement(&weightsAsymQ)];

    tensor_t weightsAsym;
    setTensorValuesForConversion(weightsAsymData, &weightsAsymQ, &weightsFloat, &weightsAsym);
    convertTensor(&weightsFloat, &weightsAsym);


    asymQConfig_t weightGradsAsymQConfig;
    initAsymQConfig(8, HTE, &weightGradsAsymQConfig);
    quantization_t weightGradsAsymQ;
    initAsymQuantization(&weightsAsymQConfig, &weightGradsAsymQ);
    uint8_t weightGrads[numberOfWeights * calcBytesPerElement(&weightGradsAsymQ)];

    parameter_t weights = {
        .data = weightsAsymData,
        .dimensions = weightDims,
        .grad = weightGrads,
        .dataQuantization = &weightsAsymQ,
        .gradQuantization = &weightGradsAsymQ,
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

    asymQConfig_t biasAsymQConfig;
    initAsymQConfig(8, HTE, &biasAsymQConfig);
    quantization_t biasAsymQ;
    initAsymQuantization(&biasAsymQConfig, &biasAsymQ);
    uint8_t biasAsymData[numberOfBiases * calcBytesPerElement(&biasAsymQ)];

    tensor_t biasAsym;
    setTensorValuesForConversion(biasAsymData, &biasAsymQ, &biasFloat, &biasAsym);
    convertTensor(&biasFloat, &biasAsym);

    asymQConfig_t biasGradsAsymQConfig;
    initAsymQConfig(8, HTE, &biasAsymQConfig);
    quantization_t biasGradsAsymQ;
    initAsymQuantization(&biasGradsAsymQConfig, &biasGradsAsymQ);
    uint8_t biasGrads[numberOfBiases*calcBytesPerElement(&biasGradsAsymQ)];

    parameter_t bias = {
        .data = biasAsymData,
        .dimensions = biasDims,
        .grad = biasGrads,
        .dataQuantization = &biasAsymQ,
        .gradQuantization = &biasGradsAsymQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims
    };

    linearConfig_t linearConfig;
    linearConfig.bias = &bias;
    linearConfig.weight = &weights;
    linearConfig.type = ASYMLAYER;

    float forwardInputFloatData[] = {0.f, 1.f, 2.f};
    size_t forwardInputDims[] = {3, 1};
    size_t forwardInputNumberOfDims = 2;
    size_t forwardInputOrderOfDims[] = {0, 1};

    quantization_t forwardInputFloatQ = {
        .type = FLOAT32
    };

    tensor_t forwardInputFloat = {
        .data = forwardInputFloatData,
        .dimensions = forwardInputDims,
        .quantization = &forwardInputFloatQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = forwardInputNumberOfDims,
        .orderOfDimensions = forwardInputOrderOfDims
    };

    asymQConfig_t forwardInputAsymQConfig;
    initAsymQConfig(8, HTE, &forwardInputAsymQConfig);
    quantization_t forwardInputAsymQ;
    initAsymQuantization(&forwardInputAsymQConfig, &forwardInputAsymQ);
    uint8_t forwardInputAsymData[numberOfForwardInputs * calcBytesPerElement(&forwardInputAsymQ)];

    tensor_t forwardInputAsym;
    setTensorValuesForConversion(forwardInputAsymData, &forwardInputAsymQ, &forwardInputFloat, &forwardInputAsym);
    convertTensor(&forwardInputFloat, &forwardInputAsym);

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

    asymQConfig_t lossAsymQConfig;
    initAsymQConfig(8, HTE, &lossAsymQConfig);
    quantization_t lossAsymQ;
    initAsymQuantization(&lossAsymQConfig, &lossAsymQ);
    uint8_t lossAsymData[numberOfLosses * calcBytesPerElement(&lossAsymQ)];

    tensor_t lossAsym;
    setTensorValuesForConversion(lossAsymData, &lossAsymQ, &lossFloat, &lossAsym);
    convertTensor(&lossFloat, &lossAsym);

    size_t propLossDims[] = {numberOfForwardInputs};
    size_t propLossNumberOfDims = 1;
    size_t propLossOrderOfDims[] = {0};

    asymQConfig_t propLossAsymQConfig;
    initAsymQConfig(8, HTE, &propLossAsymQConfig);
    quantization_t propLossAsymQ;
    initAsymQuantization(&propLossAsymQConfig, &propLossAsymQ);
    uint8_t propLossAsymData[numberOfForwardInputs * calcBytesPerElement(&propLossAsymQ)];

    tensor_t propLossAsym = {
        .data = propLossAsymData,
        .dimensions = propLossDims,
        .quantization = &propLossAsymQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = propLossNumberOfDims,
        .orderOfDimensions = propLossOrderOfDims
    };

    linearBackward(&linearConfig, &lossAsym, &forwardInputAsym, &propLossAsym);

    uint8_t weightGradsAsymData[numberOfWeights * calcBytesPerElement(&weightsAsymQ)];
    size_t weightGradsAsymOrderOfDimensions[] = {0, 1};
    tensor_t weightGradsAsym = {
        .data = weightGradsAsymData,
        .dimensions = weightDims,
        .quantization = &weightGradsAsymQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = weightNumberOfDims,
        .orderOfDimensions = weightOrderOfDims
    };
    getGradTensorFromParameter(linearConfig.weight, &weightGradsAsym,
                               weightGradsAsymOrderOfDimensions);

    float weightGradFloatData[numberOfWeights];
    quantization_t weightGradFloatQ;
    initFloat32Quantization(&weightGradFloatQ);
    tensor_t weightGradsFloat;
    setTensorValuesForConversion(weightGradFloatData, &weightGradFloatQ, &weightGradsAsym,
                                 &weightGradsFloat);
    convertTensor(&weightGradsAsym, &weightGradsFloat);

    uint8_t biasGradsAsymData[numberOfBiases * calcBytesPerElement(&biasAsymQ)];
    size_t biasGradsAsymOrderOfDimensions[] = {0, 1};
    tensor_t biasGradsAsym = {
        .data = biasGradsAsymData,
        .dimensions = biasDims,
        .quantization = &biasGradsAsymQ,
        .sparsityBitmask = NULL,
        .numberOfDimensions = biasNumberOfDims,
        .orderOfDimensions = biasOrderOfDims
    };
    getGradTensorFromParameter(linearConfig.bias, &biasGradsAsym,
                               biasGradsAsymOrderOfDimensions);

    float biasGradFloatData[2];
    quantization_t biasGradFloatQ;
    initFloat32Quantization(&biasGradFloatQ);
    tensor_t biasGradsFloat;
    setTensorValuesForConversion(biasGradFloatData, &biasGradFloatQ, &biasGradsAsym,
                                 &biasGradsFloat);

    convertTensor(&biasGradsAsym, &biasGradsFloat);

    float propLossFloatData[numberOfForwardInputs];
    quantization_t propLossFloatQ;
    initFloat32Quantization(&propLossFloatQ);
    tensor_t propLossFloat;
    setTensorValuesForConversion(propLossFloatData, &propLossFloatQ, &propLossAsym,
                                 &propLossFloat);
    convertTensor(&propLossAsym, &propLossFloat);

    size_t numberOfInputs = 3;
    float propLossArray[numberOfInputs];
    readBytesAsFloatArray(numberOfInputs, propLossFloat.data, propLossArray);
    printf("PropLoss: \n");
    for (size_t i = 0; i < numberOfInputs; i++) {
        printf("%f\n", propLossArray[i]);
    }
    printf("__________\n");

    float weightGradArray[numberOfWeights];
    readBytesAsFloatArray(numberOfWeights, weightGradsFloat.data, weightGradArray);
    printf("WeightGrads: \n");
    for (size_t i = 0; i < numberOfWeights; i++) {
        printf("%f\n", weightGradArray[i]);
    }
    printf("__________\n");

    float biasGradArray[numberOfBiases];
    readBytesAsFloatArray(numberOfBiases, biasGradsFloat.data, biasGradArray);
    printf("BiasGrads: \n");
    for (size_t i = 0; i < numberOfBiases; i++) {
        printf("%f\n", biasGradArray[i]);
    }
    printf("__________\n");

    float expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    float expected_bias_grad[] = {-4.f, -3.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propLossFloat.data,
                                  sizeof(expected_propagated_loss) / sizeof(float));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, weightGradsFloat.data,
                                  calcNumberOfElementsByDims(linearConfig.weight->numberOfDimensions
                                      , linearConfig.weight->dimensions));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad, biasGradsFloat.data,
                                  calcNumberOfElementsByDims(linearConfig.bias->numberOfDimensions,
                                      linearConfig.bias->dimensions));

}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testLinearForwardFloat);
    RUN_TEST(testLinearBackwardFloat);

    RUN_TEST(testLinearForwardAsym);
    RUN_TEST(testLinearBackwardAsym);
    UNITY_END();
}
