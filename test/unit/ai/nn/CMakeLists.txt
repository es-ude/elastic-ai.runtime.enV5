add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Linear
        MORE_LIBS
        AiHelpers
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        ReLU
        MORE_LIBS
        AiHelpers
        Quantization
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Conv1d
        MORE_LIBS
        AiHelpers
        MSE
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Softmax
        MORE_LIBS
        AiHelpers
)