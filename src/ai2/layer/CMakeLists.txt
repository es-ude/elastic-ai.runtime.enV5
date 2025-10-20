add_library(Layer
        Layer.c
)
target_include_directories(Layer PUBLIC include)
target_link_libraries(Layer PRIVATE
        Common
        DTypes
        Tensor
)

