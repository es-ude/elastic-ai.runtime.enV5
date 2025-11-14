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
        Comparison
        MORE_LIBS
        Common
        Arithmetic
        Tensor
        m
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Div
        MORE_LIBS
        Common
        Arithmetic
        Tensor
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Matmul
        MORE_LIBS
        Common
        Arithmetic
        Tensor
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Mul
        MORE_LIBS
        Common
        Arithmetic
        Tensor
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Rounding
        MORE_LIBS
        Common
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Square
        MORE_LIBS
        Common
        Arithmetic
        Tensor
)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        Sub
        MORE_LIBS
        Common
        Arithmetic
        Tensor
)