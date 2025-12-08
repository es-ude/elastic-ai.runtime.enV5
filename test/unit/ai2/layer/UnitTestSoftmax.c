#include "Softmax.h"
#include "unity.h"
#include "TensorConversion.h"
#include <stdlib.h>

void unitTestSoftmaxForwardFloat() {
    size_t inputSize = 6;

    tensor_t input;
    float inputData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t inputDims[] = {2, 3};
    size_t inputNumberOfDims = 2;
    size_t inputOrderOfDims[] = {0, 1};
    shape_t inputShape;
    setShape(&inputShape, inputDims, inputNumberOfDims, inputOrderOfDims);
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);
    setTensorValues(&input, inputData, &inputShape, &inputQ, NULL);

    tensor_t output;
    float outputData[inputSize];
    size_t outputDims[] = {2, 3};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};
    shape_t outputShape;
    setShape(&outputShape, outputDims, outputNumberOfDims, outputOrderOfDims);
    quantization_t outputQ;
    initFloat32Quantization(&outputQ);
    setTensorValues(&output, outputData, &outputShape, &outputQ, NULL);

    layer_t softmaxLayer;
    initSoftmaxLayer(&softmaxLayer);
    softmaxLayer.inputQ = &inputQ;

    softmaxForward(&softmaxLayer, &input, &output);

    float expected[] = {2.3008e-03, 6.2543e-03, 1.7001e-02, 4.6213e-02, 9.2822e-01, 1.5503e-05};

    float *actual = (float *)output.data;

    for (size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], actual[i]);
    }
}

void unitTestSoftmaxForwardAsym() {
    size_t inputSize = 6;

    tensor_t input;
    float inputData[] = {-1.f, 0.f, 1.f, 2.f, 5.f, -6.f};
    size_t inputDims[] = {2, 3};
    size_t inputNumberOfDims = 2;
    size_t inputOrderOfDims[] = {0, 1};
    shape_t inputShape;
    setShape(&inputShape, inputDims, inputNumberOfDims, inputOrderOfDims);
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);
    setTensorValues(&input, inputData, &inputShape, &inputQ, NULL);

    tensor_t inputAsym;
    asymQConfig_t inputAsymQC;
    initAsymQConfig(8, HTE, &inputAsymQC);
    quantization_t inputAsymQ;
    initAsymQuantization(&inputAsymQC, &inputAsymQ);
    uint8_t inputAsymData[inputSize];
    setTensorValuesForConversion(inputAsymData, &inputAsymQ, &input, &inputAsym);
    convertTensor(&input, &inputAsym);

    tensor_t output;
    float outputData[inputSize];
    size_t outputDims[] = {2, 3};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};
    shape_t outputShape;
    setShape(&outputShape, outputDims, outputNumberOfDims, outputOrderOfDims);
    quantization_t outputQ;
    initFloat32Quantization(&outputQ);
    setTensorValues(&output, outputData, &outputShape, &outputQ, NULL);

    tensor_t outputAsym;
    asymQConfig_t outputAsymQC;
    initAsymQConfig(8, HTE, &outputAsymQC);
    quantization_t outputAsymQ;
    initAsymQuantization(&outputAsymQC, &outputAsymQ);
    uint8_t outputAsymData[inputSize];
    setTensorValuesForConversion(outputAsymData, &outputAsymQ, &output, &outputAsym);
    convertTensor(&output, &outputAsym);

    layer_t softmaxLayer;
    initSoftmaxLayer(&softmaxLayer);
    softmaxLayer.inputQ = &inputAsymQ;

    softmaxForward(&softmaxLayer, &inputAsym, &outputAsym);

    float expected[] = {2.3008e-03, 6.2543e-03, 1.7001e-02, 4.6213e-02, 9.2822e-01, 1.5503e-05};

    convertTensor(&outputAsym, &output);

    float *actual = (float *)output.data;

    for (size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.01f, expected[i], actual[i]);
    }
}

void unitTestSoftmaxBackwardFloat() {
    size_t inputSize = 6;

    tensor_t input;
    float inputData[] = {2.3008e-03, 6.2543e-03, 1.7001e-02, 4.6213e-02, 9.2822e-01, 1.5503e-05};
    size_t inputDims[] = {2, 3};
    size_t inputNumberOfDims = 2;
    size_t inputOrderOfDims[] = {0, 1};
    shape_t inputShape;
    setShape(&inputShape, inputDims, inputNumberOfDims, inputOrderOfDims);
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);
    setTensorValues(&input, inputData, &inputShape, &inputQ, NULL);

    tensor_t loss;
    float lossData[] = {0.f, 2.f, -4.f, 6.f, 3.f, 2.f};
    size_t lossDims[] = {2, 3};
    size_t lossNumberOfDims = 2;
    size_t lossOrderOfDims[] = {0, 1};
    shape_t lossShape;
    setShape(&lossShape, lossDims, lossNumberOfDims, lossOrderOfDims);
    quantization_t lossQ;
    initFloat32Quantization(&lossQ);
    setTensorValues(&loss, lossData, &lossShape, &lossQ, NULL);

    tensor_t propLoss;
    float propLossData[inputSize];
    size_t propLossDims[] = {2, 3};
    size_t propLossNumberOfDims = 2;
    size_t propLossOrderOfDims[] = {0, 1};
    shape_t propLossShape;
    setShape(&propLossShape, propLossDims, propLossNumberOfDims, propLossOrderOfDims);
    quantization_t propLossQ;
    initFloat32Quantization(&propLossQ);
    setTensorValues(&propLoss, propLossData, &propLossShape, &propLossQ, NULL);

    layer_t softmaxLayer;
    initSoftmaxLayer(&softmaxLayer);
    softmaxLayer.inputQ = &inputQ;

    softmaxBackward(&softmaxLayer, &input, &loss, &propLoss);

    float expected[] = {-6.9173e-03, -6.2947e-03, -1.1912e-01, 1.3834e-01, -5.9973e-03,
                        -1.5603e-05};

    float *actual = (float *)propLoss.data;

    for (size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected[i], actual[i]);
    }
}

void unitTestSoftmaxBackwardAsym() {
    size_t inputSize = 6;

    tensor_t input;
    float inputData[] = {2.3008e-03, 6.2543e-03, 1.7001e-02, 4.6213e-02, 9.2822e-01, 1.5503e-05};
    size_t inputDims[] = {2, 3};
    size_t inputNumberOfDims = 2;
    size_t inputOrderOfDims[] = {0, 1};
    shape_t inputShape;
    setShape(&inputShape, inputDims, inputNumberOfDims, inputOrderOfDims);
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);
    setTensorValues(&input, inputData, &inputShape, &inputQ, NULL);

    tensor_t inputAsym;
    asymQConfig_t inputAsymQC;
    initAsymQConfig(8, HTE, &inputAsymQC);
    quantization_t inputAsymQ;
    initAsymQuantization(&inputAsymQC, &inputAsymQ);
    uint8_t inputAsymData[inputSize];
    setTensorValuesForConversion(inputAsymData, &inputAsymQ, &input, &inputAsym);
    convertTensor(&input, &inputAsym);

    tensor_t loss;
    float lossData[] = {0.f, 2.f, -4.f, 6.f, 3.f, 2.f};
    size_t lossDims[] = {2, 3};
    size_t lossNumberOfDims = 2;
    size_t lossOrderOfDims[] = {0, 1};
    shape_t lossShape;
    setShape(&lossShape, lossDims, lossNumberOfDims, lossOrderOfDims);
    quantization_t lossQ;
    initFloat32Quantization(&lossQ);
    setTensorValues(&loss, lossData, &lossShape, &lossQ, NULL);

    tensor_t lossAsym;
    asymQConfig_t lossAsymQC;
    initAsymQConfig(8, HTE, &lossAsymQC);
    quantization_t lossAsymQ;
    initAsymQuantization(&lossAsymQC, &lossAsymQ);
    uint8_t lossAsymData[inputSize];
    setTensorValuesForConversion(lossAsymData, &lossAsymQ, &loss, &lossAsym);
    convertTensor(&loss, &lossAsym);

    tensor_t propLoss;
    float propLossData[inputSize];
    size_t propLossDims[] = {2, 3};
    size_t propLossNumberOfDims = 2;
    size_t propLossOrderOfDims[] = {0, 1};
    shape_t propLossShape;
    setShape(&propLossShape, propLossDims, propLossNumberOfDims, propLossOrderOfDims);
    quantization_t propLossQ;
    initFloat32Quantization(&propLossQ);
    setTensorValues(&propLoss, propLossData, &propLossShape, &propLossQ, NULL);

    tensor_t propLossAsym;
    asymQConfig_t propLossAsymQC;
    initAsymQConfig(8, HTE, &propLossAsymQC);
    quantization_t propLossAsymQ;
    initAsymQuantization(&propLossAsymQC, &propLossAsymQ);
    uint8_t propLossAsymData[inputSize];
    setTensorValuesForConversion(propLossAsymData, &propLossAsymQ, &propLoss, &propLossAsym);
    convertTensor(&propLoss, &propLossAsym);

    layer_t softmaxLayer;
    initSoftmaxLayer(&softmaxLayer);
    softmaxLayer.inputQ = &inputAsymQ;

    softmaxBackward(&softmaxLayer, &inputAsym, &lossAsym, &propLossAsym);

    float expected[] = {-6.9173e-03, -6.2947e-03, -1.1912e-01, 1.3834e-01, -5.9973e-03,
                        -1.5603e-05};

    convertTensor(&propLossAsym, &propLoss);

    float *actual = (float *)propLoss.data;

    for (size_t i = 0; i < inputSize; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.01f, expected[i], actual[i]);
    }
}

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestSoftmaxForwardFloat);
    RUN_TEST(unitTestSoftmaxForwardAsym);

    RUN_TEST(unitTestSoftmaxBackwardFloat);
    RUN_TEST(unitTestSoftmaxBackwardAsym);
    UNITY_END();
}
