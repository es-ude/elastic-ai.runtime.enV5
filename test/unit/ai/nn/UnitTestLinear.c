#define SOURCE_FILE "LINEAR-UTEST"

#include <stdlib.h>

#include "unity.h"
#include "AiHelpers.h"
#include "Linear.h"


void setUp() {}
void tearDown() {}

linearConfig_t *initLinearConfigFloat32() {
    quantization_t *gradQ = initQuantizationByType(FLOAT32_Q);

    float32_t weights[] = {-1.f, 2.f, -3.f, 4.f, 5.f, -6.f};
    size_t weightDims[] = {2, 3};
    quantization_t *weightQ = initQuantizationByType(FLOAT32_Q);
    parameterQTensor_t *weightQTensor = initParameterQTensor(weights, 2, weightDims, weightQ, gradQ);


    float32_t bias[] = {-1.f, 3.f};
    size_t biasDims[] = {1, 2};
    quantization_t *biasQ = initQuantizationByType(FLOAT32_Q);
    parameterQTensor_t *biasQTensor = initParameterQTensor(bias, 2, biasDims, biasQ, gradQ);

    linearConfig_t *config = initLinearConfigWithWeightBias(weightQTensor, biasQTensor);

    return config;
}

void unitTestInitLinearConfigWithWeightBiasFloat32() {
    quantization_t *gradQ = initQuantizationByType(FLOAT32_Q);

    float32_t weights[] = {1.f, 2.f, 3.f, 4.f};
    size_t weightDims[] = {1, 4};
    quantization_t *weightQ = initQuantizationByType(FLOAT32_Q);
    parameterQTensor_t *weightQTensor = initParameterQTensor(weights, 2, weightDims, weightQ, gradQ);


    float32_t bias[] = {-1.f, -2.f};
    size_t biasDims[] = {1, 2};
    quantization_t *biasQ = initQuantizationByType(FLOAT32_Q);
    parameterQTensor_t *biasQTensor = initParameterQTensor(bias, 2, biasDims, biasQ, gradQ);

    linearConfig_t *linearConfig =
        initLinearConfigWithWeightBias(weightQTensor, biasQTensor);

    float32_t weightsGrad[] = {0.f, 0.f, 0.f, 0.f};
    float32_t biasGrad[] = {0.f, 0.f};

    size_t weightSize = sizeof(weights) / sizeof(float);
    size_t biasSize = sizeof(bias) / sizeof(float);

    TEST_ASSERT_EQUAL_size_t(weightSize, calcTotalNumberOfElementsByTensor(weightQTensor->dataTensor));
    TEST_ASSERT_EQUAL_size_t(biasSize, calcTotalNumberOfElementsByTensor(biasQTensor->dataTensor));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weights, linearConfig->weight->dataTensor->data, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bias, linearConfig->bias->dataTensor->data, biasSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weightsGrad, linearConfig->weight->gradTensor->data, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(biasGrad, linearConfig->bias->gradTensor->data, biasSize);
}

void unitTestInitLinearForwardWithWeightBiasFloat32() {
    quantization_t *gradQ = initQuantizationByType(FLOAT32_Q);

    float32_t weights[] = {1.f, 2.f, 3.f, 4.f};
    size_t weightDims[] = {1, 4};
    quantization_t *weightQ = initQuantizationByType(FLOAT32_Q);
    parameterQTensor_t *weightQTensor = initParameterQTensor(weights, 2, weightDims, weightQ, gradQ);


    float bias[] = {-1.f, -2.f};
    size_t biasDims[] = {1, 2};
    quantization_t *biasQ = initQuantizationByType(FLOAT32_Q);
    parameterQTensor_t *biasQTensor = initParameterQTensor(bias, 2, biasDims, biasQ, gradQ);

    layerForward_t *linearLayerForward =
        initLinearLayerForwardWithWeightBias(weightQTensor, biasQTensor);
    layerType_t layerType = LINEAR;
    TEST_ASSERT_EQUAL(layerType, linearLayerForward->type);
    TEST_ASSERT_EQUAL_PTR(linearForward, layerFunctions[linearLayerForward->type].forwardFunc);
    linearConfig_t *linearConfig = linearLayerForward->config;

    float weightsGrad[] = {0.f, 0.f, 0.f, 0.f};
    float biasGrad[] = {0.f, 0.f};

    size_t weightSize = sizeof(weights) / sizeof(float);
    size_t biasSize = sizeof(bias) / sizeof(float);

    TEST_ASSERT_EQUAL_size_t(weightSize, calcTotalNumberOfElementsByTensor(weightQTensor->dataTensor));
    TEST_ASSERT_EQUAL_size_t(biasSize, calcTotalNumberOfElementsByTensor(biasQTensor->dataTensor));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weights, linearConfig->weight->dataTensor->data, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bias, linearConfig->bias->dataTensor->data, biasSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weightsGrad, linearConfig->weight->gradTensor->data, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(biasGrad, linearConfig->bias->gradTensor->data, biasSize);
}

void unitTestInitLinearForwardBackwardWithWeightBiasFloat32() {
    quantization_t *gradQ = initQuantizationByType(FLOAT32_Q);

    float32_t weights[] = {1.f, 2.f, 3.f, 4.f};
    size_t weightDims[] = {1, 4};
    quantization_t *weightQ = initQuantizationByType(FLOAT32_Q);
    parameterQTensor_t *weightQTensor = initParameterQTensor(weights, 2, weightDims, weightQ, gradQ);

    float32_t bias[] = {-1.f, -2.f};
    size_t biasDims[] = {1, 2};
    quantization_t *biasQ = initQuantizationByType(FLOAT32_Q);
    parameterQTensor_t *biasQTensor = initParameterQTensor(bias, 2, biasDims, biasQ, gradQ);

    layerForwardBackward_t *linearLayerForwardBackward =
        initLinearLayerForwardBackwardWithWeightBias(weightQTensor, biasQTensor);
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

    TEST_ASSERT_EQUAL_size_t(weightSize, calcTotalNumberOfElementsByTensor(weightQTensor->dataTensor));
    TEST_ASSERT_EQUAL_size_t(biasSize, calcTotalNumberOfElementsByTensor(biasQTensor->dataTensor));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weights, linearConfig->weight->dataTensor->data, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bias, linearConfig->bias->dataTensor->data, biasSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(weightsGrad, linearConfig->weight->gradTensor->data, weightSize);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(biasGrad, linearConfig->bias->gradTensor->data, biasSize);
}

void unitTestLinearForwardFloat32() {
    linearConfig_t *linearConfig = initLinearConfigFloat32();
    float32_t input[] = {0.f, 1.f, 2.f};
    size_t inputDims[] = {3};
    quantization_t *inputQ = initQuantizationByType(FLOAT32_Q);
    qTensor_t *inputQTensor = initQTensor(input, 1, inputDims, inputQ);

    quantization_t *outputQ = initQuantizationByType(FLOAT32_Q);

    qTensor_t *outputQTensor = linearForward(linearConfig, inputQTensor, outputQ);
    float32_t expected_result[] = {-5.f, -4.f};
    size_t expectedOutputSize = 2;

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_result, outputQTensor->data, expectedOutputSize);
}

void unitTestLinearBackwardFloat32() {
    linearConfig_t *linearConfig = initLinearConfigFloat32();

    float32_t output[] = {0.f, 1.f, 2.f};
    size_t inputDims[] = {3};
    quantization_t *inputQ = initQuantizationByType(FLOAT32_Q);
    qTensor_t *outputQTensor = initQTensor(output, 1, inputDims, inputQ);

    float32_t loss[] = {-4.f, -3.f};
    size_t lossDims[] = {2};
    quantization_t *lossQ = initQuantizationByType(FLOAT32_Q);
    qTensor_t *lossQTensor = initQTensor(loss, 1, lossDims, lossQ);

    quantization_t *outputQ = initQuantizationByType(FLOAT32_Q);

    qTensor_t *propagated_loss = linearBackward(linearConfig, lossQTensor, outputQTensor, outputQ);

    float32_t expected_propagated_loss[] = {-8.f, -23.f, 30.f};

    float32_t expected_weight_grad[] = {0.f, -4.f, -8.f, 0.f, -3.f, -6.f};

    float32_t expected_bias_grad[] = {-4.f, -3.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss, propagated_loss->data,
                                  sizeof(expected_propagated_loss) / sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad, linearConfig->weight->gradTensor->data,
                                  calcTotalNumberOfElementsByTensor(linearConfig->weight->dataTensor));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad, linearConfig->bias->gradTensor->data,
                                  calcTotalNumberOfElementsByTensor(linearConfig->bias->dataTensor));

    /*
    qTensor_t *propagated_loss_2 = linearBackward(linearConfig, lossQTensor, outputQTensor, outputQ);

    float expected_propagated_loss_2[] = {-8.f, -23.f, 30.f};

    float expected_weight_grad_2[] = {0.f, -8.f, -16.f, 0.f, -6.f, -12.f};

    float expected_bias_grad_2[] = {-8.f, -6.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_propagated_loss_2, propagated_loss_2->data,
                                  sizeof(expected_propagated_loss_2) / sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_weight_grad_2, linearConfig->weight->gradTensor->data,
                                  calcTotalNumberOfElementsByTensor(linearConfig->weight->gradTensor));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected_bias_grad_2, linearConfig->bias->gradTensor->data,
                                  calcTotalNumberOfElementsByTensor(linearConfig->bias->gradTensor));*/
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitLinearConfigWithWeightBiasFloat32);
    RUN_TEST(unitTestInitLinearForwardWithWeightBiasFloat32);
    RUN_TEST(unitTestInitLinearForwardBackwardWithWeightBiasFloat32);
    RUN_TEST(unitTestLinearForwardFloat32);
    RUN_TEST(unitTestLinearBackwardFloat32);
    return UNITY_END();
}
