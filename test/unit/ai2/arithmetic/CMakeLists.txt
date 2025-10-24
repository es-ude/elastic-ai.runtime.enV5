add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Add
        MORE_LIBS
        Common
        Tensor
        Arithmetic
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Arithmetic
        MORE_LIBS
        Common
        Tensor
        Quantization
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Rounding
        MORE_LIBS
        Common
)