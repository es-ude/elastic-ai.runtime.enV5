#define SOURCE_FILE "LINEAR-UTEST"

#include <stdlib.h>

#include "unity.h"
#include "AiHelpers.h"
#include "Linear.h"

void setUp() {}
void tearDown() {}

linearConfig_t *initLinearConfig() {
    float weights[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    size_t weightDims[] = {6};
    float bias[] = {-1.f, 3.f};
    size_t biasDims[] = {2};

    parameterTensor_t *weightTensor = initParameterTensor(weights, 1, weightDims);
    parameterTensor_t *biasTensor = initParameterTensor(bias, 1, biasDims);

    linearConfig_t *config = initLinearConfigWithWeightBias(weightTensor, biasTensor);

    return config;
}

void unitTestInitLinearConfigWithWeightBias() {
    float weights[] = {1.f, 2.f, 3.f, 4.f};
    size_t weightDims[] = {4};
    float bias[] = {-1.f, -2.f};
    size_t biasDims[] = {2};

    parameterTensor_t *weightTensor = initParameterTensor(weights, 1, weightDims);

    parameterTensor_t *biasTensor = initParameterTensor(bias, 1, biasDims);
    linearConfig_t *linearConfig =
        initLinearConfigWithWeightBias(weightTensor, biasTensor);

    float weightsGrad[] = {0.f, 0.f, 0.f, 0.f};
    float biasGrad[] = {0.f, 0.f};

    size_t weightSize = sizeof(weights) / sizeof(float);
    size_t biasSize = sizeof(bias) / sizeof(float);

    TEST_ASSERT_EQUAL_size_t(weightSize, calcTotalNumberOfElementsByTensor(weightTensor->tensor));
    TEST_ASSERT_EQUAL_size_t(biasSize, calcTotalNumberOfElementsByTensor(biasTensor->tensor));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weights, linearConfig->weight->tensor->data, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bias, linearConfig->bias->tensor->data, biasSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weightsGrad, linearConfig->weight->grad, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(biasGrad, linearConfig->bias->grad, biasSize);
}

void unitTestInitLinearForwardWithWeightBias() {
    float weights[] = {1.f, 2.f, 3.f, 4.f};
    size_t weightDims[] = {4};
    float bias[] = {-1.f, -2.f};
    size_t biasDims[] = {2};

    parameterTensor_t *weightTensor = initParameterTensor(weights, 1, weightDims);
    parameterTensor_t *biasTensor = initParameterTensor(bias, 1, biasDims);

    layerForward_t *linearLayerForward =
        initLinearLayerForwardWithWeightBias(weightTensor, biasTensor);
    layerType_t layerType = LINEAR;
    TEST_ASSERT_EQUAL(layerType, linearLayerForward->type);
    TEST_ASSERT_EQUAL_PTR(linearForward, layerFunctions[linearLayerForward->type].forwardFunc);
    linearConfig_t *linearConfig = linearLayerForward->config;

    float weightsGrad[] = {0.f, 0.f, 0.f, 0.f};
    float biasGrad[] = {0.f, 0.f};

    size_t weightSize = sizeof(weights) / sizeof(float);
    size_t biasSize = sizeof(bias) / sizeof(float);

    TEST_ASSERT_EQUAL_size_t(weightSize, calcTotalNumberOfElementsByTensor(weightTensor->tensor));
    TEST_ASSERT_EQUAL_size_t(biasSize, calcTotalNumberOfElementsByTensor(biasTensor->tensor));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weights, linearConfig->weight->tensor->data, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bias, linearConfig->bias->tensor->data, biasSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weightsGrad, linearConfig->weight->grad, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(biasGrad, linearConfig->bias->grad, biasSize);
}

void unitTestInitLinearForwardBackwardWithWeightBias() {
    float weights[] = {1.f, 2.f, 3.f, 4.f};
    size_t weightDims[] = {4};
    float bias[] = {-1.f, -2.f};
    size_t biasDims[] = {2};

    parameterTensor_t *weightTensor = initParameterTensor(weights, 1, weightDims);
    parameterTensor_t *biasTensor = initParameterTensor(bias, 1, biasDims);

    layerForwardBackward_t *linearLayerForwardBackward =
        initLinearLayerForwardBackwardWithWeightBias(weightTensor, biasTensor);
    layerType_t layerType = LINEAR;
    TEST_ASSERT_EQUAL(layerType, linearLayerForwardBackward->type);
    TEST_ASSERT_EQUAL(&linearForward, layerFunctions[linearLayerForwardBackward->type].forwardFunc);
    TEST_ASSERT_EQUAL(&linearBackward,
                      layerFunctions[linearLayerForwardBackward->type].backwardFunc);
    linearConfig_t *linearConfig = linearLayerForwardBackward->config;

    float weightsGrad[] = {0.f, 0.f, 0.f, 0.f};
    float biasGrad[] = {0.f, 0.f};

    size_t weightSize = sizeof(weights) / sizeof(float);
    size_t biasSize = sizeof(bias) / sizeof(float);

    TEST_ASSERT_EQUAL_size_t(weightSize, calcTotalNumberOfElementsByTensor(weightTensor->tensor));
    TEST_ASSERT_EQUAL_size_t(biasSize, calcTotalNumberOfElementsByTensor(biasTensor->tensor));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weights, linearConfig->weight->tensor->data, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bias, linearConfig->bias->tensor->data, biasSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weightsGrad, linearConfig->weight->grad, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(biasGrad, linearConfig->bias->grad, biasSize);
}

void unitTestLinearForward() {
    linearConfig_t *linearConfig = initLinearConfig();
    float input[] = {0.f, 1.f, 2.f};
    size_t inputDims[] = {3};

    tensor_t *inputTensor = initTensor(input, 1, inputDims);

    tensor_t *output = linearForward(linearConfig, inputTensor);
    float expected_result[] = {-5.f, -4.f};
    size_t expectedOutputSize = 2;

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_result, output->data, expectedOutputSize);
}

void unitTestLinearBackward() {
    linearConfig_t *linearConfig = initLinearConfig();

    float input[] = {0.f, 1.f, 2.f};
    size_t inputDims[] = {3};
    tensor_t *inputTensor = initTensor(input, 1, inputDims);

    float loss[] = {-4.f, -3.f};
    size_t lossDims[] = {2};
    tensor_t *lossTensor = initTensor(loss, 1, lossDims);

    tensor_t *propagated_loss = linearBackward(linearConfig, lossTensor, inputTensor);

    float expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    float expected_bias_grad[] = {-4.f, -3.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propagated_loss->data,
                                  sizeof(expected_propagated_loss) / sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig->weight->grad,
                                  calcTotalNumberOfElementsByTensor(linearConfig->weight->tensor));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad, linearConfig->bias->grad,
                                  calcTotalNumberOfElementsByTensor(linearConfig->bias->tensor));

    tensor_t *propagated_loss_2 = linearBackward(linearConfig, lossTensor, inputTensor);

    float expected_propagated_loss_2[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad_2[] = {0.f, -8.f, -16.f, 0.f, -6.f, -12.f};

    float expected_bias_grad_2[] = {-8.f, -6.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss_2, propagated_loss_2->data,
                                  sizeof(expected_propagated_loss_2) / sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad_2, linearConfig->weight->grad,
                                  calcTotalNumberOfElementsByTensor(linearConfig->weight->tensor));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad_2, linearConfig->bias->grad,
                                  calcTotalNumberOfElementsByTensor(linearConfig->bias->tensor));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitLinearConfigWithWeightBias);
    RUN_TEST(unitTestInitLinearForwardWithWeightBias);
    RUN_TEST(unitTestInitLinearForwardBackwardWithWeightBias);
    RUN_TEST(unitTestLinearForward);
    RUN_TEST(unitTestLinearBackward);
    return UNITY_END();
}
