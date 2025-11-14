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
)

add_library(MSE
        MSE.c
)
target_include_directories(MSE PUBLIC include)
target_link_libraries(MSE PRIVATE
        Tensor
        TensorConversion
        Square
        Sub
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
)

add_library(SGD
        SGD.c
)
target_include_directories(SGD PUBLIC include)
target_link_libraries(SGD PRIVATE
        DTypes
        Tensor
        Arithmetic
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

