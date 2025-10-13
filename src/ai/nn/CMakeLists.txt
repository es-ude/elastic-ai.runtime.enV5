add_library(Linear
        Linear.c
)
target_include_directories(Linear PUBLIC include)
target_link_libraries(Linear PRIVATE
        Common
        Quantization
        AiHelpers
)

add_library(Conv1d
            Conv1d.c
)

target_include_directories(Conv1d PUBLIC include)
target_link_libraries(Conv1d PRIVATE
        Common
        Quantization
        AiHelpers
)

add_library(ReLU
        ReLU.c
)
target_include_directories(ReLU PUBLIC include)
target_link_libraries(ReLU PRIVATE
        Common
        Quantization
        AiHelpers
        Quantization
)

add_library(Softmax
        Softmax.c
)

target_include_directories(Softmax PUBLIC include)
target_link_libraries(Softmax PRIVATE
        Common
        AiHelpers
        Quantization
        m
)