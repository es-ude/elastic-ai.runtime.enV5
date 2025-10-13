add_subdirectory(helpers)
add_subdirectory(nn)
add_subdirectory(optim)
add_subdirectory(loss_functions)
add_subdirectory(quantization)

add_library(ai
        ai.c
)
target_include_directories(ai PUBLIC include)
target_link_libraries(ai PRIVATE
        Common
        AiHelpers
        Linear
        Softmax
        ReLU
        MSE
        SGD
)