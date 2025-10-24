#include "Linear.h"
#include "Tensor.h"
#include "unity.h"

#include <MinMax.h>

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
   linearConfig_t linearConfig = initLinearConfigFloat(); 

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

void testLinearBackwardFloat() {
    linearConfig_t linearConfig = initLinearConfigFloat();

    float output[] = {0.f, 1.f, 2.f};
    size_t outputDims[] = {3};
    size_t outputNumberOfDims = 1;
    size_t outputOrderOfDims[] = {0};

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

    float loss[] = {-4.f, -4.f};
    size_t lossDims[] = {2};
    size_t lossNumberOfDims = 1;
    size_t lossOrderOfDims[] = {0};

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

    float propLoss[] = {0.f, 1.f, 2.f};
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
    linearBackwardFloat(&linearConfig, &lossTensor, &outputTensor, &propLossTensor);


    float expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    float expected_bias_grad[] = {-4.f, -3.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propLossTensor.data,
                                  sizeof(expected_propagated_loss) / sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig.weight->grad,
                                  calcNumberOfElementsByDims(linearConfig.weight->numberOfDimensions, linearConfig.weight->dimensions));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad, linearConfig.bias->grad,
                                  calcNumberOfElementsByDims(linearConfig.bias->numberOfDimensions, linearConfig.bias->dimensions));


}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testLinearForwardFloat);
    RUN_TEST(testLinearBackwardFloat);
    UNITY_END();
}
