add_library(Conv1d
        Conv1d.c
)
target_include_directories(Conv1d PUBLIC include)
target_link_libraries(Conv1d PRIVATE
        DTypes
        Tensor
        Arithmetic
)

add_library(Conv1dTransposed
        Conv1dTransposed.c
)
target_include_directories(Conv1dTransposed PUBLIC include)
target_link_libraries(Conv1dTransposed PRIVATE
        DTypes
        Tensor
        Arithmetic
)

add_library(Layer
        Layer.c
)
target_include_directories(Layer PUBLIC include)
target_link_libraries(Layer PRIVATE
        DTypes
        Tensor
        Arithmetic
        Relu
)

add_library(Linear
        Linear.c
)
target_include_directories(Linear PUBLIC include)
target_link_libraries(Linear PRIVATE
        DTypes
        Tensor
        Arithmetic
        Quantization
        TensorConversion
        Layer
)

add_library(Relu
        Relu.c
)
target_include_directories(Relu PUBLIC include)
target_link_libraries(Relu PRIVATE
        DTypes
        Tensor
        Arithmetic
        Comparison
        Layer
)

add_library(Softmax
        Softmax.c
)
target_include_directories(Softmax PUBLIC include)
target_link_libraries(Softmax PRIVATE
        DTypes
        Tensor
        Arithmetic
)

add_library(CommonLayerLibs INTERFACE)
target_link_libraries(CommonLayerLibs INTERFACE
        Layer
        Linear
        Relu
        Conv1d
        Softmax
        Rounding
        Tensor
)

