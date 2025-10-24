add_library(Conv1d
        Conv1d.c
)
target_include_directories(Conv1d PUBLIC include)
target_link_libraries(Conv1d PRIVATE
        Common
        DTypes
        Tensor
        Arithmetic
)

add_library(Conv1dTransposed
        Conv1dTransposed.c
)
target_include_directories(Conv1dTransposed PUBLIC include)
target_link_libraries(Conv1dTransposed PRIVATE
        Common
        DTypes
        Tensor
        Arithmetic
)

add_library(Layer
        Layer.c
)
target_include_directories(Layer PUBLIC include)
target_link_libraries(Layer PRIVATE
        Common
        DTypes
        Tensor
        Arithmetic
)

add_library(Linear
        Linear.c
)
target_include_directories(Linear PUBLIC include)
target_link_libraries(Linear PRIVATE
        Common
        DTypes
        Tensor
        Arithmetic
        Quantization
)

add_library(Relu
        Relu.c
)
target_include_directories(Relu PUBLIC include)
target_link_libraries(Relu PRIVATE
        Common
        DTypes
        Tensor
        Arithmetic
)

add_library(Softmax
        Softmax.c
)
target_include_directories(Softmax PUBLIC include)
target_link_libraries(Softmax PRIVATE
        Common
        DTypes
        Tensor
        Arithmetic
)

