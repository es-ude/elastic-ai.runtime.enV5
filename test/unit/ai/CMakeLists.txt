add_subdirectory(helpers)
add_subdirectory(nn)
add_subdirectory(loss_functions)
add_subdirectory(optim)

add_elastic_ai_unit_test(
        LIB_UNDER_TEST
        ai
        MORE_LIBS
        Common
        AiHelpers
        Linear
        ReLU
        MSE
        SGD
)